/***********************************************************\
 * Macros to auto-ish generate struct/enum descriptions    *
 *     ___________ _____                                   *
 *    /  ___|  ___/ __  \                                  *
 *    \ `--.| |_  `' / /'                                  *
 *     `--. \  _|   / /                                    *
 *    /\__/ / |   ./ /___                                  *
 *    \____/\_|   \_____/                                  *
 *                                                         *
 *                                                         *
 *  Copyright (c) 2014 Florian Oetke                       *
 *                                                         *
 *  This file is part of SF2 and distributed under         *
 *  the MIT License. See LICENSE file for details.         *
\***********************************************************/

#pragma once

#include "reflection_data.hpp"

namespace sf2 {

}


/* This counts the number of args */
#define SF2_NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_100,_101,_102,_103,_104,_105,_106,_107,_108,_109,_110,_111,_112,_113,_114,_115,_116,_117,_118,_119,_120,_121,_122,_123,_124,_125,_126,_127,_128,_129,_130,_131,_132,_133,_134,_135,_136,_137,_138,_139,_140,_141,_142,_143,_144,_145,_146,_147,_148,_149,_150,_151,_152,_153,_154,_155,_156,_157,_158,_159,_160,_161,_162,_163,_164,_165,_166,_167,_168,_169,_170,_171,_172,_173,_174,_175,_176,_177,_178,_179,_180,_181,_182,_183,_184,_185,_186,_187,_188,_189,_190,_191,_192,_193,_194,_195,_196,_197,_198,_199,_200,_201,_202,_203,_204,_205,_206,_207,_208,_209,_210,_211,_212,_213,_214,_215,_216,_217,_218,_219,_220,_221,_222,_223,_224,_225,_226,_227,_228,_229,_230,_231,_232,_233,_234,_235,_236,_237,_238,_239,_240,_241,_242,_243,_244,_245,_246,_247,_248,_249,_250,_251,_252,_253,_254,_255,_256,N,...) N

#define SF2_NARGS(...) SF2_NARGS_SEQ(__VA_ARGS__, 256, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/* This will let macros expand before concating them */
#define SF2_PRIMITIVE_CAT(x, y) x ## y
#define SF2_CAT(x, y) SF2_PRIMITIVE_CAT(x, y)

/* This will call a macro on each argument passed in */
#define SF2_APPLY(macro, ...) SF2_CAT(SF2_APPLY_, SF2_NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define SF2_APPLY_1(m, x1) m(x1)
#define SF2_APPLY_2(m, x, ...) m(x), SF2_APPLY_1(m,__VA_ARGS__)
#define SF2_APPLY_3(m, x, ...) m(x), SF2_APPLY_2(m,__VA_ARGS__)
#define SF2_APPLY_4(m, x, ...) m(x), SF2_APPLY_3(m,__VA_ARGS__)
#define SF2_APPLY_5(m, x, ...) m(x), SF2_APPLY_4(m,__VA_ARGS__)
#define SF2_APPLY_6(m, x, ...) m(x), SF2_APPLY_5(m,__VA_ARGS__)
#define SF2_APPLY_7(m, x, ...) m(x), SF2_APPLY_6(m,__VA_ARGS__)
#define SF2_APPLY_8(m, x, ...) m(x), SF2_APPLY_7(m,__VA_ARGS__)
#define SF2_APPLY_9(m, x, ...) m(x), SF2_APPLY_8(m,__VA_ARGS__)
#define SF2_APPLY_10(m, x, ...) m(x), SF2_APPLY_9(m,__VA_ARGS__)
#define SF2_APPLY_11(m, x, ...) m(x), SF2_APPLY_10(m,__VA_ARGS__)
#define SF2_APPLY_12(m, x, ...) m(x), SF2_APPLY_11(m,__VA_ARGS__)
#define SF2_APPLY_13(m, x, ...) m(x), SF2_APPLY_12(m,__VA_ARGS__)
#define SF2_APPLY_14(m, x, ...) m(x), SF2_APPLY_13(m,__VA_ARGS__)
#define SF2_APPLY_15(m, x, ...) m(x), SF2_APPLY_14(m,__VA_ARGS__)
#define SF2_APPLY_16(m, x, ...) m(x), SF2_APPLY_15(m,__VA_ARGS__)
#define SF2_APPLY_17(m, x, ...) m(x), SF2_APPLY_16(m,__VA_ARGS__)
#define SF2_APPLY_18(m, x, ...) m(x), SF2_APPLY_17(m,__VA_ARGS__)
#define SF2_APPLY_19(m, x, ...) m(x), SF2_APPLY_18(m,__VA_ARGS__)
#define SF2_APPLY_20(m, x, ...) m(x), SF2_APPLY_19(m,__VA_ARGS__)
#define SF2_APPLY_21(m, x, ...) m(x), SF2_APPLY_20(m,__VA_ARGS__)
#define SF2_APPLY_22(m, x, ...) m(x), SF2_APPLY_21(m,__VA_ARGS__)
#define SF2_APPLY_23(m, x, ...) m(x), SF2_APPLY_22(m,__VA_ARGS__)
#define SF2_APPLY_24(m, x, ...) m(x), SF2_APPLY_23(m,__VA_ARGS__)
#define SF2_APPLY_25(m, x, ...) m(x), SF2_APPLY_24(m,__VA_ARGS__)
#define SF2_APPLY_26(m, x, ...) m(x), SF2_APPLY_25(m,__VA_ARGS__)
#define SF2_APPLY_27(m, x, ...) m(x), SF2_APPLY_26(m,__VA_ARGS__)
#define SF2_APPLY_28(m, x, ...) m(x), SF2_APPLY_27(m,__VA_ARGS__)
#define SF2_APPLY_29(m, x, ...) m(x), SF2_APPLY_28(m,__VA_ARGS__)
#define SF2_APPLY_30(m, x, ...) m(x), SF2_APPLY_29(m,__VA_ARGS__)
#define SF2_APPLY_31(m, x, ...) m(x), SF2_APPLY_30(m,__VA_ARGS__)
#define SF2_APPLY_32(m, x, ...) m(x), SF2_APPLY_31(m,__VA_ARGS__)
#define SF2_APPLY_33(m, x, ...) m(x), SF2_APPLY_32(m,__VA_ARGS__)
#define SF2_APPLY_34(m, x, ...) m(x), SF2_APPLY_33(m,__VA_ARGS__)
#define SF2_APPLY_35(m, x, ...) m(x), SF2_APPLY_34(m,__VA_ARGS__)
#define SF2_APPLY_36(m, x, ...) m(x), SF2_APPLY_35(m,__VA_ARGS__)
#define SF2_APPLY_37(m, x, ...) m(x), SF2_APPLY_36(m,__VA_ARGS__)
#define SF2_APPLY_38(m, x, ...) m(x), SF2_APPLY_37(m,__VA_ARGS__)
#define SF2_APPLY_39(m, x, ...) m(x), SF2_APPLY_38(m,__VA_ARGS__)
#define SF2_APPLY_40(m, x, ...) m(x), SF2_APPLY_39(m,__VA_ARGS__)
#define SF2_APPLY_41(m, x, ...) m(x), SF2_APPLY_40(m,__VA_ARGS__)
#define SF2_APPLY_42(m, x, ...) m(x), SF2_APPLY_41(m,__VA_ARGS__)
#define SF2_APPLY_43(m, x, ...) m(x), SF2_APPLY_42(m,__VA_ARGS__)
#define SF2_APPLY_44(m, x, ...) m(x), SF2_APPLY_43(m,__VA_ARGS__)
#define SF2_APPLY_45(m, x, ...) m(x), SF2_APPLY_44(m,__VA_ARGS__)
#define SF2_APPLY_46(m, x, ...) m(x), SF2_APPLY_45(m,__VA_ARGS__)
#define SF2_APPLY_47(m, x, ...) m(x), SF2_APPLY_46(m,__VA_ARGS__)
#define SF2_APPLY_48(m, x, ...) m(x), SF2_APPLY_47(m,__VA_ARGS__)
#define SF2_APPLY_49(m, x, ...) m(x), SF2_APPLY_48(m,__VA_ARGS__)
#define SF2_APPLY_50(m, x, ...) m(x), SF2_APPLY_49(m,__VA_ARGS__)
#define SF2_APPLY_51(m, x, ...) m(x), SF2_APPLY_50(m,__VA_ARGS__)
#define SF2_APPLY_52(m, x, ...) m(x), SF2_APPLY_51(m,__VA_ARGS__)
#define SF2_APPLY_53(m, x, ...) m(x), SF2_APPLY_52(m,__VA_ARGS__)
#define SF2_APPLY_54(m, x, ...) m(x), SF2_APPLY_53(m,__VA_ARGS__)
#define SF2_APPLY_55(m, x, ...) m(x), SF2_APPLY_54(m,__VA_ARGS__)
#define SF2_APPLY_56(m, x, ...) m(x), SF2_APPLY_55(m,__VA_ARGS__)
#define SF2_APPLY_57(m, x, ...) m(x), SF2_APPLY_56(m,__VA_ARGS__)
#define SF2_APPLY_58(m, x, ...) m(x), SF2_APPLY_57(m,__VA_ARGS__)
#define SF2_APPLY_59(m, x, ...) m(x), SF2_APPLY_58(m,__VA_ARGS__)
#define SF2_APPLY_60(m, x, ...) m(x), SF2_APPLY_59(m,__VA_ARGS__)
#define SF2_APPLY_61(m, x, ...) m(x), SF2_APPLY_60(m,__VA_ARGS__)
#define SF2_APPLY_62(m, x, ...) m(x), SF2_APPLY_61(m,__VA_ARGS__)
#define SF2_APPLY_63(m, x, ...) m(x), SF2_APPLY_62(m,__VA_ARGS__)
#define SF2_APPLY_64(m, x, ...) m(x), SF2_APPLY_63(m,__VA_ARGS__)
#define SF2_APPLY_65(m, x, ...) m(x), SF2_APPLY_64(m,__VA_ARGS__)
#define SF2_APPLY_66(m, x, ...) m(x), SF2_APPLY_65(m,__VA_ARGS__)
#define SF2_APPLY_67(m, x, ...) m(x), SF2_APPLY_66(m,__VA_ARGS__)
#define SF2_APPLY_68(m, x, ...) m(x), SF2_APPLY_67(m,__VA_ARGS__)
#define SF2_APPLY_69(m, x, ...) m(x), SF2_APPLY_68(m,__VA_ARGS__)
#define SF2_APPLY_70(m, x, ...) m(x), SF2_APPLY_69(m,__VA_ARGS__)
#define SF2_APPLY_71(m, x, ...) m(x), SF2_APPLY_70(m,__VA_ARGS__)
#define SF2_APPLY_72(m, x, ...) m(x), SF2_APPLY_71(m,__VA_ARGS__)
#define SF2_APPLY_73(m, x, ...) m(x), SF2_APPLY_72(m,__VA_ARGS__)
#define SF2_APPLY_74(m, x, ...) m(x), SF2_APPLY_73(m,__VA_ARGS__)
#define SF2_APPLY_75(m, x, ...) m(x), SF2_APPLY_74(m,__VA_ARGS__)
#define SF2_APPLY_76(m, x, ...) m(x), SF2_APPLY_75(m,__VA_ARGS__)
#define SF2_APPLY_77(m, x, ...) m(x), SF2_APPLY_76(m,__VA_ARGS__)
#define SF2_APPLY_78(m, x, ...) m(x), SF2_APPLY_77(m,__VA_ARGS__)
#define SF2_APPLY_79(m, x, ...) m(x), SF2_APPLY_78(m,__VA_ARGS__)
#define SF2_APPLY_80(m, x, ...) m(x), SF2_APPLY_79(m,__VA_ARGS__)
#define SF2_APPLY_81(m, x, ...) m(x), SF2_APPLY_80(m,__VA_ARGS__)
#define SF2_APPLY_82(m, x, ...) m(x), SF2_APPLY_81(m,__VA_ARGS__)
#define SF2_APPLY_83(m, x, ...) m(x), SF2_APPLY_82(m,__VA_ARGS__)
#define SF2_APPLY_84(m, x, ...) m(x), SF2_APPLY_83(m,__VA_ARGS__)
#define SF2_APPLY_85(m, x, ...) m(x), SF2_APPLY_84(m,__VA_ARGS__)
#define SF2_APPLY_86(m, x, ...) m(x), SF2_APPLY_85(m,__VA_ARGS__)
#define SF2_APPLY_87(m, x, ...) m(x), SF2_APPLY_86(m,__VA_ARGS__)
#define SF2_APPLY_88(m, x, ...) m(x), SF2_APPLY_87(m,__VA_ARGS__)
#define SF2_APPLY_89(m, x, ...) m(x), SF2_APPLY_88(m,__VA_ARGS__)
#define SF2_APPLY_90(m, x, ...) m(x), SF2_APPLY_89(m,__VA_ARGS__)
#define SF2_APPLY_91(m, x, ...) m(x), SF2_APPLY_90(m,__VA_ARGS__)
#define SF2_APPLY_92(m, x, ...) m(x), SF2_APPLY_91(m,__VA_ARGS__)
#define SF2_APPLY_93(m, x, ...) m(x), SF2_APPLY_92(m,__VA_ARGS__)
#define SF2_APPLY_94(m, x, ...) m(x), SF2_APPLY_93(m,__VA_ARGS__)
#define SF2_APPLY_95(m, x, ...) m(x), SF2_APPLY_94(m,__VA_ARGS__)
#define SF2_APPLY_96(m, x, ...) m(x), SF2_APPLY_95(m,__VA_ARGS__)
#define SF2_APPLY_97(m, x, ...) m(x), SF2_APPLY_96(m,__VA_ARGS__)
#define SF2_APPLY_98(m, x, ...) m(x), SF2_APPLY_97(m,__VA_ARGS__)
#define SF2_APPLY_99(m, x, ...) m(x), SF2_APPLY_98(m,__VA_ARGS__)
#define SF2_APPLY_100(m, x, ...) m(x), SF2_APPLY_99(m,__VA_ARGS__)
#define SF2_APPLY_101(m, x, ...) m(x), SF2_APPLY_100(m,__VA_ARGS__)
#define SF2_APPLY_102(m, x, ...) m(x), SF2_APPLY_101(m,__VA_ARGS__)
#define SF2_APPLY_103(m, x, ...) m(x), SF2_APPLY_102(m,__VA_ARGS__)
#define SF2_APPLY_104(m, x, ...) m(x), SF2_APPLY_103(m,__VA_ARGS__)
#define SF2_APPLY_105(m, x, ...) m(x), SF2_APPLY_104(m,__VA_ARGS__)
#define SF2_APPLY_106(m, x, ...) m(x), SF2_APPLY_105(m,__VA_ARGS__)
#define SF2_APPLY_107(m, x, ...) m(x), SF2_APPLY_106(m,__VA_ARGS__)
#define SF2_APPLY_108(m, x, ...) m(x), SF2_APPLY_107(m,__VA_ARGS__)
#define SF2_APPLY_109(m, x, ...) m(x), SF2_APPLY_108(m,__VA_ARGS__)
#define SF2_APPLY_110(m, x, ...) m(x), SF2_APPLY_109(m,__VA_ARGS__)
#define SF2_APPLY_111(m, x, ...) m(x), SF2_APPLY_110(m,__VA_ARGS__)
#define SF2_APPLY_112(m, x, ...) m(x), SF2_APPLY_111(m,__VA_ARGS__)
#define SF2_APPLY_113(m, x, ...) m(x), SF2_APPLY_112(m,__VA_ARGS__)
#define SF2_APPLY_114(m, x, ...) m(x), SF2_APPLY_113(m,__VA_ARGS__)
#define SF2_APPLY_115(m, x, ...) m(x), SF2_APPLY_114(m,__VA_ARGS__)
#define SF2_APPLY_116(m, x, ...) m(x), SF2_APPLY_115(m,__VA_ARGS__)
#define SF2_APPLY_117(m, x, ...) m(x), SF2_APPLY_116(m,__VA_ARGS__)
#define SF2_APPLY_118(m, x, ...) m(x), SF2_APPLY_117(m,__VA_ARGS__)
#define SF2_APPLY_119(m, x, ...) m(x), SF2_APPLY_118(m,__VA_ARGS__)
#define SF2_APPLY_120(m, x, ...) m(x), SF2_APPLY_119(m,__VA_ARGS__)
#define SF2_APPLY_121(m, x, ...) m(x), SF2_APPLY_120(m,__VA_ARGS__)
#define SF2_APPLY_122(m, x, ...) m(x), SF2_APPLY_121(m,__VA_ARGS__)
#define SF2_APPLY_123(m, x, ...) m(x), SF2_APPLY_122(m,__VA_ARGS__)
#define SF2_APPLY_124(m, x, ...) m(x), SF2_APPLY_123(m,__VA_ARGS__)
#define SF2_APPLY_125(m, x, ...) m(x), SF2_APPLY_124(m,__VA_ARGS__)
#define SF2_APPLY_126(m, x, ...) m(x), SF2_APPLY_125(m,__VA_ARGS__)
#define SF2_APPLY_127(m, x, ...) m(x), SF2_APPLY_126(m,__VA_ARGS__)
#define SF2_APPLY_128(m, x, ...) m(x), SF2_APPLY_127(m,__VA_ARGS__)
#define SF2_APPLY_129(m, x, ...) m(x), SF2_APPLY_128(m,__VA_ARGS__)
#define SF2_APPLY_130(m, x, ...) m(x), SF2_APPLY_129(m,__VA_ARGS__)
#define SF2_APPLY_131(m, x, ...) m(x), SF2_APPLY_130(m,__VA_ARGS__)
#define SF2_APPLY_132(m, x, ...) m(x), SF2_APPLY_131(m,__VA_ARGS__)
#define SF2_APPLY_133(m, x, ...) m(x), SF2_APPLY_132(m,__VA_ARGS__)
#define SF2_APPLY_134(m, x, ...) m(x), SF2_APPLY_133(m,__VA_ARGS__)
#define SF2_APPLY_135(m, x, ...) m(x), SF2_APPLY_134(m,__VA_ARGS__)
#define SF2_APPLY_136(m, x, ...) m(x), SF2_APPLY_135(m,__VA_ARGS__)
#define SF2_APPLY_137(m, x, ...) m(x), SF2_APPLY_136(m,__VA_ARGS__)
#define SF2_APPLY_138(m, x, ...) m(x), SF2_APPLY_137(m,__VA_ARGS__)
#define SF2_APPLY_139(m, x, ...) m(x), SF2_APPLY_138(m,__VA_ARGS__)
#define SF2_APPLY_140(m, x, ...) m(x), SF2_APPLY_139(m,__VA_ARGS__)
#define SF2_APPLY_141(m, x, ...) m(x), SF2_APPLY_140(m,__VA_ARGS__)
#define SF2_APPLY_142(m, x, ...) m(x), SF2_APPLY_141(m,__VA_ARGS__)
#define SF2_APPLY_143(m, x, ...) m(x), SF2_APPLY_142(m,__VA_ARGS__)
#define SF2_APPLY_144(m, x, ...) m(x), SF2_APPLY_143(m,__VA_ARGS__)
#define SF2_APPLY_145(m, x, ...) m(x), SF2_APPLY_144(m,__VA_ARGS__)
#define SF2_APPLY_146(m, x, ...) m(x), SF2_APPLY_145(m,__VA_ARGS__)
#define SF2_APPLY_147(m, x, ...) m(x), SF2_APPLY_146(m,__VA_ARGS__)
#define SF2_APPLY_148(m, x, ...) m(x), SF2_APPLY_147(m,__VA_ARGS__)
#define SF2_APPLY_149(m, x, ...) m(x), SF2_APPLY_148(m,__VA_ARGS__)
#define SF2_APPLY_150(m, x, ...) m(x), SF2_APPLY_149(m,__VA_ARGS__)
#define SF2_APPLY_151(m, x, ...) m(x), SF2_APPLY_150(m,__VA_ARGS__)
#define SF2_APPLY_152(m, x, ...) m(x), SF2_APPLY_151(m,__VA_ARGS__)
#define SF2_APPLY_153(m, x, ...) m(x), SF2_APPLY_152(m,__VA_ARGS__)
#define SF2_APPLY_154(m, x, ...) m(x), SF2_APPLY_153(m,__VA_ARGS__)
#define SF2_APPLY_155(m, x, ...) m(x), SF2_APPLY_154(m,__VA_ARGS__)
#define SF2_APPLY_156(m, x, ...) m(x), SF2_APPLY_155(m,__VA_ARGS__)
#define SF2_APPLY_157(m, x, ...) m(x), SF2_APPLY_156(m,__VA_ARGS__)
#define SF2_APPLY_158(m, x, ...) m(x), SF2_APPLY_157(m,__VA_ARGS__)
#define SF2_APPLY_159(m, x, ...) m(x), SF2_APPLY_158(m,__VA_ARGS__)
#define SF2_APPLY_160(m, x, ...) m(x), SF2_APPLY_159(m,__VA_ARGS__)
#define SF2_APPLY_161(m, x, ...) m(x), SF2_APPLY_160(m,__VA_ARGS__)
#define SF2_APPLY_162(m, x, ...) m(x), SF2_APPLY_161(m,__VA_ARGS__)
#define SF2_APPLY_163(m, x, ...) m(x), SF2_APPLY_162(m,__VA_ARGS__)
#define SF2_APPLY_164(m, x, ...) m(x), SF2_APPLY_163(m,__VA_ARGS__)
#define SF2_APPLY_165(m, x, ...) m(x), SF2_APPLY_164(m,__VA_ARGS__)
#define SF2_APPLY_166(m, x, ...) m(x), SF2_APPLY_165(m,__VA_ARGS__)
#define SF2_APPLY_167(m, x, ...) m(x), SF2_APPLY_166(m,__VA_ARGS__)
#define SF2_APPLY_168(m, x, ...) m(x), SF2_APPLY_167(m,__VA_ARGS__)
#define SF2_APPLY_169(m, x, ...) m(x), SF2_APPLY_168(m,__VA_ARGS__)
#define SF2_APPLY_170(m, x, ...) m(x), SF2_APPLY_169(m,__VA_ARGS__)
#define SF2_APPLY_171(m, x, ...) m(x), SF2_APPLY_170(m,__VA_ARGS__)
#define SF2_APPLY_172(m, x, ...) m(x), SF2_APPLY_171(m,__VA_ARGS__)
#define SF2_APPLY_173(m, x, ...) m(x), SF2_APPLY_172(m,__VA_ARGS__)
#define SF2_APPLY_174(m, x, ...) m(x), SF2_APPLY_173(m,__VA_ARGS__)
#define SF2_APPLY_175(m, x, ...) m(x), SF2_APPLY_174(m,__VA_ARGS__)
#define SF2_APPLY_176(m, x, ...) m(x), SF2_APPLY_175(m,__VA_ARGS__)
#define SF2_APPLY_177(m, x, ...) m(x), SF2_APPLY_176(m,__VA_ARGS__)
#define SF2_APPLY_178(m, x, ...) m(x), SF2_APPLY_177(m,__VA_ARGS__)
#define SF2_APPLY_179(m, x, ...) m(x), SF2_APPLY_178(m,__VA_ARGS__)
#define SF2_APPLY_180(m, x, ...) m(x), SF2_APPLY_179(m,__VA_ARGS__)
#define SF2_APPLY_181(m, x, ...) m(x), SF2_APPLY_180(m,__VA_ARGS__)
#define SF2_APPLY_182(m, x, ...) m(x), SF2_APPLY_181(m,__VA_ARGS__)
#define SF2_APPLY_183(m, x, ...) m(x), SF2_APPLY_182(m,__VA_ARGS__)
#define SF2_APPLY_184(m, x, ...) m(x), SF2_APPLY_183(m,__VA_ARGS__)
#define SF2_APPLY_185(m, x, ...) m(x), SF2_APPLY_184(m,__VA_ARGS__)
#define SF2_APPLY_186(m, x, ...) m(x), SF2_APPLY_185(m,__VA_ARGS__)
#define SF2_APPLY_187(m, x, ...) m(x), SF2_APPLY_186(m,__VA_ARGS__)
#define SF2_APPLY_188(m, x, ...) m(x), SF2_APPLY_187(m,__VA_ARGS__)
#define SF2_APPLY_189(m, x, ...) m(x), SF2_APPLY_188(m,__VA_ARGS__)
#define SF2_APPLY_190(m, x, ...) m(x), SF2_APPLY_189(m,__VA_ARGS__)
#define SF2_APPLY_191(m, x, ...) m(x), SF2_APPLY_190(m,__VA_ARGS__)
#define SF2_APPLY_192(m, x, ...) m(x), SF2_APPLY_191(m,__VA_ARGS__)
#define SF2_APPLY_193(m, x, ...) m(x), SF2_APPLY_192(m,__VA_ARGS__)
#define SF2_APPLY_194(m, x, ...) m(x), SF2_APPLY_193(m,__VA_ARGS__)
#define SF2_APPLY_195(m, x, ...) m(x), SF2_APPLY_194(m,__VA_ARGS__)
#define SF2_APPLY_196(m, x, ...) m(x), SF2_APPLY_195(m,__VA_ARGS__)
#define SF2_APPLY_197(m, x, ...) m(x), SF2_APPLY_196(m,__VA_ARGS__)
#define SF2_APPLY_198(m, x, ...) m(x), SF2_APPLY_197(m,__VA_ARGS__)
#define SF2_APPLY_199(m, x, ...) m(x), SF2_APPLY_198(m,__VA_ARGS__)
#define SF2_APPLY_200(m, x, ...) m(x), SF2_APPLY_199(m,__VA_ARGS__)
#define SF2_APPLY_201(m, x, ...) m(x), SF2_APPLY_200(m,__VA_ARGS__)
#define SF2_APPLY_202(m, x, ...) m(x), SF2_APPLY_201(m,__VA_ARGS__)
#define SF2_APPLY_203(m, x, ...) m(x), SF2_APPLY_202(m,__VA_ARGS__)
#define SF2_APPLY_204(m, x, ...) m(x), SF2_APPLY_203(m,__VA_ARGS__)
#define SF2_APPLY_205(m, x, ...) m(x), SF2_APPLY_204(m,__VA_ARGS__)
#define SF2_APPLY_206(m, x, ...) m(x), SF2_APPLY_205(m,__VA_ARGS__)
#define SF2_APPLY_207(m, x, ...) m(x), SF2_APPLY_206(m,__VA_ARGS__)
#define SF2_APPLY_208(m, x, ...) m(x), SF2_APPLY_207(m,__VA_ARGS__)
#define SF2_APPLY_209(m, x, ...) m(x), SF2_APPLY_208(m,__VA_ARGS__)
#define SF2_APPLY_210(m, x, ...) m(x), SF2_APPLY_209(m,__VA_ARGS__)
#define SF2_APPLY_211(m, x, ...) m(x), SF2_APPLY_210(m,__VA_ARGS__)
#define SF2_APPLY_212(m, x, ...) m(x), SF2_APPLY_211(m,__VA_ARGS__)
#define SF2_APPLY_213(m, x, ...) m(x), SF2_APPLY_212(m,__VA_ARGS__)
#define SF2_APPLY_214(m, x, ...) m(x), SF2_APPLY_213(m,__VA_ARGS__)
#define SF2_APPLY_215(m, x, ...) m(x), SF2_APPLY_214(m,__VA_ARGS__)
#define SF2_APPLY_216(m, x, ...) m(x), SF2_APPLY_215(m,__VA_ARGS__)
#define SF2_APPLY_217(m, x, ...) m(x), SF2_APPLY_216(m,__VA_ARGS__)
#define SF2_APPLY_218(m, x, ...) m(x), SF2_APPLY_217(m,__VA_ARGS__)
#define SF2_APPLY_219(m, x, ...) m(x), SF2_APPLY_218(m,__VA_ARGS__)
#define SF2_APPLY_220(m, x, ...) m(x), SF2_APPLY_219(m,__VA_ARGS__)
#define SF2_APPLY_221(m, x, ...) m(x), SF2_APPLY_220(m,__VA_ARGS__)
#define SF2_APPLY_222(m, x, ...) m(x), SF2_APPLY_221(m,__VA_ARGS__)
#define SF2_APPLY_223(m, x, ...) m(x), SF2_APPLY_222(m,__VA_ARGS__)
#define SF2_APPLY_224(m, x, ...) m(x), SF2_APPLY_223(m,__VA_ARGS__)
#define SF2_APPLY_225(m, x, ...) m(x), SF2_APPLY_224(m,__VA_ARGS__)
#define SF2_APPLY_226(m, x, ...) m(x), SF2_APPLY_225(m,__VA_ARGS__)
#define SF2_APPLY_227(m, x, ...) m(x), SF2_APPLY_226(m,__VA_ARGS__)
#define SF2_APPLY_228(m, x, ...) m(x), SF2_APPLY_227(m,__VA_ARGS__)
#define SF2_APPLY_229(m, x, ...) m(x), SF2_APPLY_228(m,__VA_ARGS__)
#define SF2_APPLY_230(m, x, ...) m(x), SF2_APPLY_229(m,__VA_ARGS__)
#define SF2_APPLY_231(m, x, ...) m(x), SF2_APPLY_230(m,__VA_ARGS__)
#define SF2_APPLY_232(m, x, ...) m(x), SF2_APPLY_231(m,__VA_ARGS__)
#define SF2_APPLY_233(m, x, ...) m(x), SF2_APPLY_232(m,__VA_ARGS__)
#define SF2_APPLY_234(m, x, ...) m(x), SF2_APPLY_233(m,__VA_ARGS__)
#define SF2_APPLY_235(m, x, ...) m(x), SF2_APPLY_234(m,__VA_ARGS__)
#define SF2_APPLY_236(m, x, ...) m(x), SF2_APPLY_235(m,__VA_ARGS__)
#define SF2_APPLY_237(m, x, ...) m(x), SF2_APPLY_236(m,__VA_ARGS__)
#define SF2_APPLY_238(m, x, ...) m(x), SF2_APPLY_237(m,__VA_ARGS__)
#define SF2_APPLY_239(m, x, ...) m(x), SF2_APPLY_238(m,__VA_ARGS__)
#define SF2_APPLY_240(m, x, ...) m(x), SF2_APPLY_239(m,__VA_ARGS__)
#define SF2_APPLY_241(m, x, ...) m(x), SF2_APPLY_240(m,__VA_ARGS__)
#define SF2_APPLY_242(m, x, ...) m(x), SF2_APPLY_241(m,__VA_ARGS__)
#define SF2_APPLY_243(m, x, ...) m(x), SF2_APPLY_242(m,__VA_ARGS__)
#define SF2_APPLY_244(m, x, ...) m(x), SF2_APPLY_243(m,__VA_ARGS__)
#define SF2_APPLY_245(m, x, ...) m(x), SF2_APPLY_244(m,__VA_ARGS__)
#define SF2_APPLY_246(m, x, ...) m(x), SF2_APPLY_245(m,__VA_ARGS__)
#define SF2_APPLY_247(m, x, ...) m(x), SF2_APPLY_246(m,__VA_ARGS__)
#define SF2_APPLY_248(m, x, ...) m(x), SF2_APPLY_247(m,__VA_ARGS__)
#define SF2_APPLY_249(m, x, ...) m(x), SF2_APPLY_248(m,__VA_ARGS__)
#define SF2_APPLY_250(m, x, ...) m(x), SF2_APPLY_249(m,__VA_ARGS__)
#define SF2_APPLY_251(m, x, ...) m(x), SF2_APPLY_250(m,__VA_ARGS__)
#define SF2_APPLY_252(m, x, ...) m(x), SF2_APPLY_251(m,__VA_ARGS__)
#define SF2_APPLY_253(m, x, ...) m(x), SF2_APPLY_252(m,__VA_ARGS__)
#define SF2_APPLY_254(m, x, ...) m(x), SF2_APPLY_253(m,__VA_ARGS__)
#define SF2_APPLY_255(m, x, ...) m(x), SF2_APPLY_254(m,__VA_ARGS__)
#define SF2_APPLY_256(m, x, ...) m(x), SF2_APPLY_255(m,__VA_ARGS__)


#define SF2_EXTRACT_VALUE(n) ::std::make_pair(sf2_current_type::n, ::sf2::String_literal{#n})

#define sf2_enumDef(TYPE, ...) inline auto& sf2_enum_info_factory(TYPE*) {\
	using sf2_current_type = TYPE;\
	static const auto data = ::sf2::Enum_info<TYPE>{\
			::sf2::String_literal{#TYPE}, \
			{SF2_APPLY(SF2_EXTRACT_VALUE,__VA_ARGS__)}\
	};\
	return data;\
}


#define SF2_EXTRACT_TYPE(n) decltype(sf2_current_type::n)
#define SF2_EXTRACT_MEMBER(n) ::std::make_pair(&sf2_current_type::n, ::sf2::String_literal{#n})

#define sf2_structDef(TYPE, ...) inline auto& sf2_struct_info_factory(TYPE*) {\
	using sf2_current_type = TYPE;\
	static constexpr auto data = ::sf2::Struct_info<TYPE,\
		SF2_APPLY(SF2_EXTRACT_TYPE,__VA_ARGS__)>{\
			::sf2::String_literal{#TYPE}, \
			SF2_APPLY(SF2_EXTRACT_MEMBER,__VA_ARGS__)\
	};\
	return data;\
}

#define sf2_accesor(TYPE) auto& sf2_struct_info_factory(TYPE*)

