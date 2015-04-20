#include "serializer.hpp"

#include "../asset/asset_manager.hpp"
#include "ecs.hpp"
#include "../utils/template_utils.hpp"
#include "serializer_impl.hpp"

#include <unordered_map>
#include <vector>
#include <sf2/sf2.hpp>
#include <iostream>


namespace core {
	namespace ecs {
		using namespace asset;
		using namespace sf2;
		using namespace sf2::io;

		class Blueprint : public std::enable_shared_from_this<const Blueprint> {
			public:
				struct Data;

				Blueprint(AID name, Data&& data);
				~Blueprint()noexcept;
				Blueprint& operator=(Blueprint&&);

				void apply(Entity_ptr target)const;
				void detach(Entity_ptr target)const;


				const AID name;

			private:
				struct PImpl;
				std::unique_ptr<PImpl> _impl;

				Blueprint(const Blueprint&) = delete;
				Blueprint(Blueprint&& s) = delete;
				Blueprint& operator=(const Blueprint&) = delete;
		};

		/// connects a entity to its assigned blueprint
		class BlueprintComponent : public ecs::Component<BlueprintComponent> {
			public:
				static constexpr const char* name() {return "Blueprint";}
				static void load(sf2::io::CharSource& cs, ecs::details::Component_base& comp){
					std::string blueprintName;
					sf2::details::fundamentals::_parseMember(cs, blueprintName);

					auto& self = static_cast<BlueprintComponent&>(comp);
					self.blueprintId = AID{Asset_type::blueprint, blueprintName};
				};
				static void store(sf2::io::CharSink& cs, const ecs::details::Component_base& comp){
					auto& self = static_cast<const BlueprintComponent&>(comp);
					sf2::details::fundamentals::_writeMember(cs, self.blueprintId.name());
				}

				BlueprintComponent(ecs::Entity& owner, std::shared_ptr<const Blueprint> blueprint=std::shared_ptr<const Blueprint>())noexcept
				  : Component(owner), blueprint(blueprint), blueprintId(blueprint ? blueprint->name : AID{Asset_type::blueprint, ""}) {}
				~BlueprintComponent()noexcept {
					if(blueprint) {
						blueprint->detach(owner_ptr());
						blueprint.reset();
					}
				}

				BlueprintComponent& operator=(BlueprintComponent&& o) noexcept {
					ecs::Component<BlueprintComponent>::operator=(static_cast<Component&&>(o));

					blueprint = std::move(o.blueprint); // deatch/attach not required, Comp-move accors entities is undefined
					blueprintId = std::move(blueprintId);
					return *this;
				}

				void set(std::shared_ptr<const Blueprint> blueprint) {
					this->blueprint = blueprint;
					blueprintId = blueprint->name;
				}

				std::shared_ptr<const Blueprint> blueprint;
				AID blueprintId;
		};

		// used to store/load the blueprint informations
		struct Blueprint::Data {
			std::string contentStr;
		};

		// private members
		struct Blueprint::PImpl {
			Data data;
			mutable std::vector<Entity*> users;
		};


		Blueprint::Blueprint(AID name, Blueprint::Data&& data) : name(name), _impl(new PImpl{std::move(data), {}}) {
		}
		Blueprint::~Blueprint()noexcept {
		}

		namespace {
			void onError(std::string msg, sf2::io::CharSource& cs) {
				ERROR(msg<<" at "<<cs.line()<<":"<<cs.column()<<"; nextChar="<<static_cast<int>(cs()));
			}

			template<class CompTypeRange>
			void storeEntity(const CompTypeRange& comps,
							 sf2::io::CharSink& sink, Entity& e) {
				sink('{');

				bool first=true;
				for(auto&& comp : comps) {
					const details::Component_type_info& compInfo = comp.second;

					auto compPtr = compInfo.get(e);
					if(compPtr!=nullptr) {
						if(first) first=false;
						else sink(',');

						sink<<compInfo.name;

						Entity_state state {sink};
						compPtr->store(state);
					}
				}

				sink('}');
			}

			void restoreEntity(sf2::io::CharSource& cs, Entity& e) {
				char c=cs();

				if(!skipComment(c,cs))	return;

				if( c!='{' ) {
					onError(std::string("Blueprint definition has to start with '{', found: ")+c, cs);
					return;
				}

				char memberBuffer[sf2::details::MAX_MEMBER_NAME_LENGTH];

				while( c!='}' ) {
					WeakStringRef id = sf2::details::readIdentifier(cs, memberBuffer);
					c=cs.prev();

					if( id.begin==id.end ) {
						onError(std::string("invalid identifier in object '")+id.toString()+"', next char: "+c, cs);
						return;
					}

					if(!skipComment(c,cs)) {
						onError(std::string("Blueprint definition has to end with '}', found: ")+c, cs);
						return;
					}

					if( c!=':' && c!=',' && c!='}' ) {
						onError("missing ':', ',' or '}' after member name '"+id.toString()+"', found: "+c, cs);
						return;
					}

					auto comp = e.manager().comp_info(id.toString());

					auto cPtr = comp.get(e);
					if(cPtr==nullptr) {
						comp.add(e);
						cPtr = comp.get(e);
					}

					if(c==':') {
						Entity_state state{cs};
						cPtr->load(state);
						c = cs.prev();
						if(c==':')
							c = cs();
					}

					if(!skipComment(c,cs)) {
						onError(std::string("Blueprint definition has to end with '}', found: ")+c, cs);
						return;
					}
				}

				if( c!='}' ) {
					onError(std::string("Blueprint definition has to end with '}', found: ")+c, cs);
					return;
				}
			}

			void applyBlueprint(const Blueprint::Data& data, Entity& e) {
				StringCharSource cs(data.contentStr);
				restoreEntity(cs, e);
			}
		}

		Blueprint& Blueprint::operator=(Blueprint&& o) {
			// swap data but keep user-list
			std::swap(_impl, o._impl);
			_impl->users = std::move(o._impl->users);

			for(auto&& u : _impl->users)
				applyBlueprint(_impl->data, *u);

			return *this;
		}

		void Blueprint::apply(Entity_ptr target)const {
			if(!_impl)return;

			if(!target->has<BlueprintComponent>())
				target->emplace<BlueprintComponent>(shared_from_this());
			else
				target->get<BlueprintComponent>().get_or_throw().set(shared_from_this());

			_impl->users.push_back(target.get());

			applyBlueprint(_impl->data, *target);
		}
		void Blueprint::detach(Entity_ptr target)const {
			if(!_impl)return;

			util::erase_fast(_impl->users, target.get());
		}


	//-----------------------------------------------------------


		Serializer::Serializer(Entity_manager& entityMgr, asset::Asset_manager& assetMgr)
			: _entities(entityMgr), _assets(assetMgr) {
			static bool initialized = false;
			if(!initialized) {
				initialized = true;
				entityMgr.register_component_type<BlueprintComponent>();
			}
		}

		Entity_ptr Serializer::apply(const asset::AID& blueprint, Entity_ptr target)const {
			_assets.load<Blueprint>(blueprint)->apply(target);
			return target;
		}
		void Serializer::detach(Entity_ptr target)const {
			target->erase<BlueprintComponent>();
		}

		void Serializer::on_reload() {
			for(auto&& comp : _entities.list<BlueprintComponent>()) {
				if(!comp.blueprintId) {
					apply(comp.blueprintId, comp.owner_ptr());
				}
			}
		}

		std::string Serializer::write() {
			const auto compTypes = _entities.list_comp_infos();

			io::StrCharSink sink;

			sink('[');

			bool first=true;
			for(auto& e : _entities.list_entities()) {
				if(first) first=false;
				else sink(',');

				storeEntity(compTypes, sink, *e);
			}

			sink(']');

			return sink.extractString();
		}

		void Serializer::read(const std::string& content) {
			_entities.clear();

			StringCharSource cs(content);

			char c=cs();

			if(!skipComment(c,cs))	return;

			if( c!='[' ) {
				onError(std::string("EntityManager definition has to start with '[', found: ")+c, cs);
				return;
			}

			while( c!=']' ) {
				auto entity = _entities.emplace();
				restoreEntity(cs, *entity);

				c = cs();

				if(!skipComment(c,cs)) {
					onError(std::string("EntityManager definition has to end with ']', found: ")+c, cs);
					return;
				}
			}

			if( c!=']' ) {
				onError(std::string("EntityManager definition has to end with ']', found: ")+c, cs);
				return;
			}
		}
	}

	namespace asset {
		template<>
		struct Loader<ecs::Blueprint> {
			using RT = std::shared_ptr<ecs::Blueprint>;

			static RT load(istream in) throw(Loading_failed){
				return std::make_shared<ecs::Blueprint>(in.aid(), ecs::Blueprint::Data{in.content()});
			}

			static void store(ostream out, ecs::Blueprint& asset) throw(Loading_failed) {
				// TODO
				FAIL("NOT IMPLEMENTED, YET!");
			}
		};
	}

}

