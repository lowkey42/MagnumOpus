#include "mapping.hpp"

#include <sf2/sf2.hpp>

namespace mo {
namespace sys {
namespace controller {

	sf2_enumDef(Command,
		sf2_value(move_up),
		sf2_value(move_down),
		sf2_value(move_left),
		sf2_value(move_right),
		sf2_value(attack),
		sf2_value(use_or_take),
		sf2_value(weapon_0),
		sf2_value(weapon_1),
		sf2_value(weapon_2),
		sf2_value(weapon_3),
		sf2_value(enter_leave_game),
		sf2_value(quit)
	)

	sf2_enumDef(Key,
		sf2_value(Return),
		sf2_value(Escape),
		sf2_value(Backspace),
		sf2_value(Tab),
		sf2_value(Space),
		sf2_value(Exclaim),
		sf2_value(Quotedbl),
		sf2_value(Hash),
		sf2_value(Percent),
		sf2_value(Dollar),
		sf2_value(Ampersand),
		sf2_value(Quote),
		sf2_value(LeftParan),
		sf2_value(RightParan),
		sf2_value(Asterisk),
		sf2_value(Plus),
		sf2_value(Comma),
		sf2_value(Minus),
		sf2_value(Period),
		sf2_value(Slash),
		sf2_value(T_0),
		sf2_value(T_1),
		sf2_value(T_2),
		sf2_value(T_3),
		sf2_value(T_4),
		sf2_value(T_5),
		sf2_value(T_6),
		sf2_value(T_7),
		sf2_value(T_8),
		sf2_value(T_9),
		sf2_value(Colon),
		sf2_value(Semicolon),
		sf2_value(Less),
		sf2_value(Equals),
		sf2_value(Greater),
		sf2_value(Question),
		sf2_value(At),
		sf2_value(LeftBracket),
		sf2_value(RightBracket),
		sf2_value(Backslash),
		sf2_value(Caret),
		sf2_value(Underscore),
		sf2_value(Backquote),
		sf2_value(A),
		sf2_value(B),
		sf2_value(C),
		sf2_value(D),
		sf2_value(E),
		sf2_value(F),
		sf2_value(G),
		sf2_value(H),
		sf2_value(I),
		sf2_value(J),
		sf2_value(K),
		sf2_value(L),
		sf2_value(M),
		sf2_value(N),
		sf2_value(O),
		sf2_value(P),
		sf2_value(Q),
		sf2_value(R),
		sf2_value(S),
		sf2_value(T),
		sf2_value(U),
		sf2_value(V),
		sf2_value(W),
		sf2_value(X),
		sf2_value(Y),
		sf2_value(Z),
		sf2_value(Capslock),

		sf2_value(F1),
		sf2_value(F2),
		sf2_value(F3),
		sf2_value(F4),
		sf2_value(F5),
		sf2_value(F6),
		sf2_value(F7),
		sf2_value(F8),
		sf2_value(F9),
		sf2_value(F10),
		sf2_value(F11),
		sf2_value(F12),
		sf2_value(PrintScreen),
		sf2_value(Scrolllock),
		sf2_value(Pause),

		sf2_value(Left),
		sf2_value(Right),
		sf2_value(Up),
		sf2_value(Down),

		sf2_value(KP_0),
		sf2_value(KP_1),
		sf2_value(KP_2),
		sf2_value(KP_3),
		sf2_value(KP_4),
		sf2_value(KP_5),
		sf2_value(KP_6),
		sf2_value(KP_7),
		sf2_value(KP_8),
		sf2_value(KP_9),

		sf2_value(LControl),
		sf2_value(LShift),
		sf2_value(LAlt),
		sf2_value(LSuper),
		sf2_value(RControl),
		sf2_value(RShift),
		sf2_value(RAlt),
		sf2_value(RSuper)
	)

	sf2_enumDef(Pad_stick,
		sf2_value(left),
		sf2_value(right)
	)

	sf2_enumDef(Pad_button,
		sf2_value(a),
		sf2_value(b),
		sf2_value(x),
		sf2_value(y),
		sf2_value(back),
		sf2_value(guide),
		sf2_value(start),
		sf2_value(left_stick),
		sf2_value(right_stick),
		sf2_value(left_shoulder),
		sf2_value(right_shoulder),
		sf2_value(d_pad_up),
		sf2_value(d_pad_down),
		sf2_value(d_pad_left),
		sf2_value(d_pad_right),
		sf2_value(left_trigger),
		sf2_value(right_trigger)
	)

	sf2_structDef(Mapping,
		sf2_member(mouse_look),
		sf2_member(key_action),
		sf2_member(mouse_button_action),
		sf2_member(pad_button_action),
		sf2_member(aim_sticks),
		sf2_member(move_sticks),
		sf2_member(stick_dead_zone)
	)
}
}
namespace asset {

	std::shared_ptr<sys::controller::Mapping> Loader<sys::controller::Mapping>::load(istream in) throw(Loading_failed){
		auto r = std::make_shared<sys::controller::Mapping>();

		std::string data = in.content();
		sf2::io::StringCharSource source(data);

		sf2::ParserDefChooser<sys::controller::Mapping>::get().parse(source, *r.get());

		return r;
	}

	void Loader<sys::controller::Mapping>::store(ostream out, sys::controller::Mapping& asset) throw(Loading_failed) {
		std::string data = sf2::writeString(asset);
		out.write(data.c_str(), data.length());
	}
}
}
