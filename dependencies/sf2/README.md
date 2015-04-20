sf2 - Simple and Fast Struct Format
===

This is a experimental serializer &amp; deserializer for C++ data structures

The format is very similar to JSON but not completely compatible.


Things it does:
* Semi-automatically handles structs and enum classes
* Automatically handles all build in types and the following STL-classes:
    * unique_ptr
    * shared_ptr
    * vector
    * map
* Can write & read from/to strings and files
* Stateless parsers/writes, all instances are (a) generated in-place or (b) constructed in local buffers and moved afterwards


Things it does not do:
* Read/ write Unicode or other non-ASCII encodings
* Conserve comments after one read-write cycle
* Some usefull STL-classes (e.g. weak_ptr, set, list, hash_map, ...)
* Automatic indentation/ formating


===
Examples:

``` cpp
enum class Color {
	RED, GREEN, BLUE
};
sf2_enumDef(Color,
	sf2_value(RED),
	sf2_value(GREEN),
	sf2_value(BLUE)
);

struct Position {
	float x, y, z;
};
sf2_structDef(Position,
	sf2_member(x),
	sf2_member(y),
	sf2_member(z)
);

struct Player {
	Position position;
	Color color;
	std::string name;
};
sf2_structDef(Player,
	sf2_member(position),
	sf2_member(color),
	sf2_member(name)
);

// ...

Player player;

std::string playerAsString = sf2::writeString(player);

Player playerFromString = sf2::parseString<Player>(playerAsString);
```

