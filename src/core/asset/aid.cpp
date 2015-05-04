#include "aid.hpp"

#include "../utils/string_utils.hpp"
#include "../utils/log.hpp"

namespace mo {
namespace asset {

	AID::AID(Asset_type t, std::string n)
	    : _type(t), _name(std::make_shared<std::string>(std::move(n))) {
	}

	AID::AID(std::string n) : _type(Asset_type::gen) {
		auto r = util::split(n, ":");
		util::to_lower_inplace(r.first);
		util::to_lower_inplace(r.second);
		_name = std::make_shared<std::string>(std::move(r.second));

		if     ("gen"==r.first)          _type=Asset_type::gen;
		else if("tex"==r.first)          _type=Asset_type::tex;
		else if("vert_shader"==r.first)  _type=Asset_type::vert_shader;
		else if("frag_shader"==r.first)  _type=Asset_type::frag_shader;
		else if("sound"==r.first)        _type=Asset_type::sound;
		else if("music"==r.first)        _type=Asset_type::music;
		else if("cfg"==r.first)          _type=Asset_type::cfg;
		else if("blueprint"==r.first)    _type=Asset_type::blueprint;
		else if("font"==r.first)         _type=Asset_type::font;
		else if("anim"==r.first)         _type=Asset_type::anim;
		else FAIL("Unknown AssetClass: "<<r.first);
	}

	std::string AID::str()const noexcept {
		std::string class_name;

		switch(_type) {
			case Asset_type::gen:         class_name="gen";         break;
			case Asset_type::tex:         class_name="tex";         break;
			case Asset_type::vert_shader: class_name="vert_shader"; break;
			case Asset_type::frag_shader: class_name="frag_shader"; break;
			case Asset_type::sound:       class_name="sound";       break;
			case Asset_type::music:       class_name="music";       break;
			case Asset_type::cfg:         class_name="cfg";         break;
			case Asset_type::blueprint:   class_name="blueprint";   break;
			case Asset_type::font:        class_name="font";        break;
			case Asset_type::anim:        class_name="anim";        break;
			default: FAIL("Unknown AssetType: "<<static_cast<uint16_t>(_type));
		}

		return class_name+":"+*_name;
	}

	bool AID::operator==(const AID& o)const noexcept {
		return _type==o._type && (_name==o._name || *_name==*o._name);
	}
	bool AID::operator!=(const AID& o)const noexcept {
		return !(*this==o);
	}
	bool AID::operator<(const AID& o)const noexcept {
		return _type<o._type || *_name<*o._name;
	}
	AID::operator bool()const noexcept {
		return _name && !_name->empty();
	}

}
}
