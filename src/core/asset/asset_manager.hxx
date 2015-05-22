#pragma once

#ifndef ASSETMANAGER_INCLUDED
#include "asset_manager.hpp"
#endif


namespace mo {
namespace asset {

	template<class T>
	void Asset_manager::_asset_reloader_impl(void* asset, istream in) throw(Loading_failed) {
		auto newAsset = Loader<T>::load(std::move(in));
		*static_cast<T*>(asset) = std::move(*newAsset.get());
	}

	template<typename T>
	Ptr<T> Asset_manager::load(const AID& id) throw(Loading_failed) {
		auto asset = load_maybe<T>(id);

		if(asset.is_nothing())
			throw Loading_failed("asset not found: "+id.str());

		return asset.get_or_throw();
	}

	template<typename T>
	auto Asset_manager::load_maybe(const AID& id) throw(Loading_failed) -> util::maybe<Ptr<T>> {
		auto res = _assets.find(id);
		if(res!=_assets.end())
			return Ptr<T>{*this, id, std::static_pointer_cast<const T>(res->second.data)};

		auto path = _locate(id);

		if(!path)
			return util::nothing();

		auto stream = _open(path.get_or_throw(), id);
		if(!stream)
			return util::nothing();

		auto asset = Loader<T>::load(std::move(stream.get_or_throw()));

		_add_asset(id, path.get_or_throw(), &_asset_reloader_impl<T>, std::static_pointer_cast<void>(asset));

		return Ptr<T>{*this, id, asset};
	}

	template<typename T>
	void Asset_manager::save(const AID& id, const T& asset) throw(Loading_failed) {
		Loader<T>::store(_create(id), asset);
		_assets.erase(id);
	}


	template<class R>
	Ptr<R>::Ptr() : _mgr(nullptr) {}

	template<class R>
	Ptr<R>::Ptr(Asset_manager& mgr, const AID& id, std::shared_ptr<const R> res)
	    : _mgr(&mgr), _ptr(res), _aid(id) {}

    template<class R>
    const R& Ptr<R>::operator*(){
        load();
        return *_ptr.get();
    }
    template<class R>
    const R& Ptr<R>::operator*()const {
        INVARIANT(*this, "Access to unloaded resource");
        return *_ptr.get();
    }

	template<class R>
	const R* Ptr<R>::operator->(){
		load();
		return _ptr.get();
	}
	template<class R>
	const R* Ptr<R>::operator->()const {
		INVARIANT(*this, "Access to unloaded resource");
		return _ptr.get();
	}

	template<class R>
	bool Ptr<R>::operator==(const Ptr& o)const noexcept {
		return _aid == o._aid;
	}
	template<class R>
	bool Ptr<R>::operator<(const Ptr& o)const noexcept {
		return _aid < o._aid;
	}

	template<class R>
	Ptr<R>::operator std::shared_ptr<const R>() {
		load();
		return _ptr;
	}

	template<class R>
	void Ptr<R>::load() {
		if(!_ptr) {
			INVARIANT(_mgr, "Tried to load unintialized resource-ref");
			INVARIANT(_aid, "Tried to load unnamed resource");
			*this = _mgr->load<R>(_aid);
		}
	}

	template<class R>
	void Ptr<R>::unload() {
		_ptr.reset();
	}

}
}
