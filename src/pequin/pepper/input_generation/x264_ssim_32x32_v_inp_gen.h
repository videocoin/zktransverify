

void x264_ssim_32x32_input_gen(mpq_t * input_q, int num_inputs, char *argv[]) {
    
    unsigned char a[1024] = {
            176,
            148,
            80,
            189,
            172,
            143,
            200,
            211,
            172,
            164,
            211,
            211,
            172,
            164,
            211,
            211,
            172,
            164,
            211,
            211,
            172,
            164,
            211,
            211,
            172,
            164,
            211,
            211,
            172,
            164,
            211,
            211,
            187,
            104,
            128,
            207,
            170,
            166,
            220,
            213,
            171,
            167,
            220,
            213,
            171,
            167,
            220,
            213,
            171,
            167,
            220,
            214,
            171,
            167,
            220,
            213,
            171,
            167,
            220,
            213,
            170,
            169,
            219,
            213,
            172,
            115,
            196,
            213,
            169,
            166,
            220,
            213,
            169,
            166,
            220,
            213,
            169,
            166,
            220,
            213,
            169,
            165,
            221,
            213,
            169,
            165,
            220,
            213,
            169,
            164,
            220,
            212,
            188,
            199,
            213,
            212,
            167,
            138,
            218,
            212,
            168,
            165,
            219,
            212,
            168,
            164,
            219,
            212,
            168,
            165,
            219,
            213,
            168,
            176,
            210,
            140,
            143,
            180,
            198,
            205,
            169,
            179,
            223,
            204,
            216,
            216,
            209,
            211,
            164,
            153,
            219,
            211,
            166,
            162,
            219,
            211,
            166,
            165,
            216,
            213,
            167,
            162,
            220,
            207,
            149,
            196,
            182,
            93,
            127,
            197,
            177,
            151,
            202,
            227,
            219,
            194,
            203,
            219,
            208,
            211,
            162,
            157,
            218,
            211,
            165,
            161,
            218,
            211,
            164,
            170,
            212,
            180,
            163,
            168,
            216,
            136,
            127,
            196,
            179,
            95,
            124,
            200,
            186,
            172,
            218,
            229,
            215,
            175,
            194,
            221,
            208,
            207,
            160,
            155,
            218,
            210,
            163,
            159,
            218,
            210,
            163,
            167,
            216,
            148,
            134,
            193,
            183,
            92,
            125,
            195,
            178,
            95,
            126,
            212,
            198,
            194,
            189,
            222,
            212,
            170,
            205,
            224,
            206,
            130,
            158,
            153,
            217,
            209,
            161,
            157,
            217,
            209,
            161,
            168,
            215,
            119,
            124,
            196,
            177,
            93,
            122,
            201,
            186,
            158,
            195,
            228,
            213,
            178,
            179,
            222,
            212,
            174,
            215,
            213,
            194,
            87,
            156,
            151,
            217,
            208,
            159,
            155,
            217,
            208,
            159,
            164,
            216,
            117,
            122,
            194,
            177,
            89,
            140,
            212,
            199,
            179,
            222,
            230,
            216,
            170,
            179,
            222,
            212,
            190,
            194,
            197,
            179,
            89,
            154,
            149,
            216,
            207,
            157,
            153,
            216,
            208,
            148,
            187,
            187,
            88,
            121,
            198,
            181,
            132,
            150,
            148,
            187,
            158,
            217,
            231,
            216,
            173,
            180,
            228,
            214,
            176,
            140,
            194,
            176,
            89,
            153,
            147,
            216,
            207,
            155,
            151,
            215,
            208,
            157,
            203,
            207,
            154,
            186,
            226,
            209,
            196,
            147,
            116,
            173,
            101,
            200,
            233,
            217,
            191,
            209,
            233,
            215,
            151,
            121,
            194,
            176,
            87,
            151,
            145,
            216,
            206,
            151,
            152,
            219,
            202,
            201,
            230,
            218,
            198,
            222,
            233,
            216,
            204,
            129,
            108,
            173,
            143,
            214,
            232,
            216,
            195,
            220,
            232,
            215,
            169,
            135,
            193,
            175,
            87,
            148,
            143,
            215,
            205,
            164,
            205,
            220,
            195,
            220,
            233,
            216,
            194,
            219,
            232,
            216,
            199,
            177,
            165,
            201,
            186,
            221,
            232,
            216,
            194,
            219,
            230,
            213,
            178,
            155,
            192,
            174,
            85,
            144,
            138,
            219,
            202,
            203,
            213,
            210,
            185,
            219,
            232,
            216,
            194,
            219,
            232,
            216,
            194,
            220,
            232,
            217,
            195,
            219,
            232,
            216,
            194,
            217,
            229,
            211,
            171,
            140,
            192,
            174,
            83,
            141,
            144,
            225,
            195,
            214,
            220,
            209,
            177,
            215,
            232,
            216,
            194,
            219,
            232,
            216,
            194,
            219,
            232,
            216,
            194,
            219,
            232,
            216,
            194,
            211,
            229,
            209,
            151,
            120,
            192,
            173,
            82,
            133,
            161,
            225,
            192,
            212,
            222,
            213,
            186,
            215,
            232,
            216,
            193,
            213,
            232,
            216,
            193,
            218,
            232,
            216,
            193,
            218,
            232,
            216,
            190,
            206,
            228,
            207,
            122,
            112,
            192,
            173,
            80,
            120,
            155,
            225,
            193,
            199,
            173,
            207,
            189,
            220,
            233,
            216,
            151,
            189,
            233,
            216,
            193,
            218,
            233,
            215,
            192,
            219,
            232,
            215,
            181,
            203,
            228,
            207,
            105,
            109,
            191,
            172,
            77,
            121,
            136,
            226,
            195,
            207,
            182,
            208,
            192,
            223,
            226,
            202,
            87,
            198,
            233,
            217,
            194,
            221,
            233,
            214,
            194,
            213,
            231,
            211,
            175,
            210,
            231,
            211,
            109,
            121,
            193,
            173,
            78,
            124,
            121,
            225,
            198,
            222,
            229,
            216,
            198,
            175,
            212,
            175,
            90,
            219,
            230,
            212,
            197,
            189,
            181,
            210,
            199,
            190,
            231,
            210,
            182,
            217,
            232,
            214,
            160,
            152,
            194,
            173,
            81,
            126,
            120,
            225,
            200,
            224,
            231,
            217,
            136,
            97,
            210,
            170,
            131,
            224,
            179,
            206,
            201,
            128,
            121,
            214,
            201,
            192,
            232,
            211,
            195,
            199,
            225,
            207,
            200,
            160,
            195,
            174,
            84,
            128,
            121,
            219,
            203,
            167,
            169,
            216,
            143,
            135,
            217,
            183,
            178,
            187,
            126,
            213,
            203,
            128,
            123,
            217,
            202,
            201,
            233,
            215,
            202,
            200,
            228,
            211,
            186,
            162,
            200,
            177,
            87,
            130,
            122,
            215,
            206,
            127,
            123,
            216,
            209,
            182,
            212,
            209,
            204,
            139,
            125,
            215,
            205,
            130,
            124,
            220,
            203,
            213,
            234,
            220,
            207,
            226,
            234,
            220,
            175,
            178,
            211,
            188,
            93,
            132,
            124,
            216,
            207,
            133,
            129,
            216,
            207,
            135,
            138,
            215,
            207,
            131,
            129,
            216,
            207,
            133,
            126,
            222,
            207,
            226,
            235,
            222,
            209,
            230,
            234,
            222,
            181,
            156,
            167,
            200,
            124,
            134,
            126,
            216,
            208,
            134,
            130,
            216,
            208,
            133,
            129,
            216,
            208,
            134,
            130,
            216,
            208,
            134,
            128,
            226,
            209,
            230,
            235,
            223,
            212,
            227,
            232,
            216,
            197,
            165,
            149,
            198,
            155,
            136,
            128,
            217,
            209,
            137,
            133,
            217,
            209,
            137,
            133,
            217,
            209,
            137,
            133,
            217,
            209,
            136,
            133,
            228,
            212,
            230,
            235,
            224,
            215,
            227,
            232,
            214,
            209,
            186,
            159,
            202,
            165,
            139,
            131,
            217,
            209,
            139,
            135,
            217,
            209,
            139,
            135,
            217,
            209,
            139,
            135,
            217,
            209,
            139,
            136,
            228,
            214,
            231,
            235,
            226,
            218,
            231,
            234,
            225,
            217,
            218,
            211,
            219,
            162,
            142,
            133,
            218,
            210,
            142,
            138,
            218,
            210,
            142,
            138,
            218,
            210,
            142,
            138,
            218,
            210,
            142,
            136,
            227,
            216,
            232,
            235,
            228,
            221,
            232,
            235,
            228,
            221,
            232,
            230,
            215,
            118,
            144,
            131,
            218,
            211,
            145,
            140,
            218,
            211,
            145,
            140,
            218,
            211,
            145,
            140,
            218,
            211,
            145,
            138,
            224,
            216,
            231,
            235,
            229,
            224,
            233,
            235,
            229,
            224,
            230,
            226,
            201,
            114,
            146,
            117,
            217,
            212,
            148,
            144,
            219,
            212,
            148,
            144,
            219,
            212,
            148,
            144,
            219,
            212,
            148,
            141,
            221,
            214,
            222,
            235,
            231,
            227,
            233,
            235,
            231,
            228,
            230,
            227,
            204,
            120,
            144,
            96,
            194,
            213,
            151,
            146,
            219,
            213,
            151,
            146,
            219,
            213,
            151,
            146,
            219,
            213,
            151,
            146,
            219,
            213,
            206,
            235,
            233,
            231,
            234,
            235,
            233,
            231,
            234,
            231,
            219,
            130,
            130,
            85,
            126,
            207,
            155,
            148,
            219,
            214,
            155,
            149,
            219,
            214,
            155,
            149,
            219,
            214,
            155,
            149,
            219,
            213,
            180,
            230,
            234,
            233,
            234,
            235,
            234,
            233,
            234,
            234,
            232,
            170,
            140,
            94,
            70,
            186,
            155,
            126,
            199,
            213,
            158,
            148,
            211,
            213,
            158,
            148,
            211,
            213,
            158,
            148,
            211,
            213,
            165,
            215,
            226,
            233,
            235,
            230,
            226,
            233,
            235,
            230,
            226,
            212
    };

    unsigned char b[1024] = {
            175,
            148,
            79,
            188,
            171,
            142,
            200,
            211,
            171,
            163,
            211,
            211,
            171,
            163,
            211,
            211,
            171,
            163,
            211,
            211,
            171,
            163,
            211,
            211,
            171,
            163,
            211,
            211,
            171,
            162,
            211,
            211,
            186,
            103,
            127,
            207,
            169,
            165,
            220,
            213,
            170,
            166,
            220,
            213,
            170,
            166,
            220,
            213,
            170,
            166,
            220,
            213,
            170,
            166,
            220,
            213,
            170,
            166,
            219,
            213,
            169,
            168,
            219,
            213,
            171,
            114,
            196,
            212,
            168,
            165,
            219,
            212,
            168,
            165,
            219,
            212,
            168,
            165,
            219,
            212,
            168,
            164,
            220,
            213,
            168,
            164,
            220,
            213,
            168,
            163,
            220,
            212,
            187,
            197,
            212,
            212,
            166,
            137,
            218,
            212,
            167,
            164,
            219,
            212,
            167,
            164,
            219,
            212,
            167,
            163,
            219,
            213,
            167,
            175,
            210,
            139,
            142,
            180,
            198,
            204,
            168,
            178,
            223,
            203,
            216,
            215,
            209,
            211,
            163,
            152,
            219,
            211,
            165,
            162,
            219,
            211,
            165,
            164,
            216,
            212,
            166,
            161,
            220,
            206,
            148,
            196,
            182,
            94,
            127,
            196,
            178,
            150,
            201,
            227,
            218,
            193,
            202,
            219,
            208,
            210,
            161,
            156,
            218,
            210,
            164,
            161,
            218,
            210,
            164,
            169,
            211,
            179,
            162,
            167,
            216,
            135,
            127,
            196,
            179,
            96,
            124,
            199,
            186,
            170,
            218,
            228,
            214,
            174,
            193,
            221,
            208,
            206,
            159,
            155,
            218,
            209,
            162,
            158,
            218,
            210,
            162,
            166,
            216,
            147,
            134,
            193,
            184,
            93,
            125,
            195,
            178,
            96,
            126,
            211,
            198,
            193,
            189,
            222,
            212,
            168,
            204,
            224,
            206,
            130,
            157,
            152,
            217,
            208,
            160,
            157,
            217,
            208,
            160,
            167,
            215,
            118,
            124,
            196,
            177,
            94,
            122,
            201,
            186,
            157,
            194,
            228,
            212,
            177,
            179,
            222,
            212,
            173,
            214,
            212,
            194,
            89,
            155,
            151,
            217,
            208,
            158,
            154,
            217,
            208,
            158,
            163,
            215,
            117,
            122,
            194,
            177,
            90,
            139,
            212,
            199,
            178,
            222,
            230,
            215,
            168,
            178,
            222,
            212,
            188,
            193,
            196,
            180,
            91,
            154,
            148,
            216,
            207,
            156,
            153,
            216,
            208,
            148,
            186,
            186,
            90,
            121,
            198,
            181,
            131,
            149,
            148,
            187,
            157,
            217,
            231,
            215,
            172,
            180,
            228,
            214,
            175,
            139,
            193,
            176,
            90,
            152,
            147,
            216,
            206,
            154,
            150,
            215,
            207,
            156,
            203,
            206,
            154,
            185,
            226,
            208,
            195,
            146,
            117,
            173,
            102,
            199,
            233,
            216,
            190,
            208,
            233,
            214,
            151,
            121,
            194,
            176,
            89,
            150,
            144,
            216,
            206,
            150,
            152,
            219,
            202,
            200,
            230,
            217,
            197,
            222,
            233,
            215,
            203,
            128,
            110,
            174,
            142,
            213,
            232,
            216,
            194,
            219,
            232,
            214,
            167,
            135,
            192,
            175,
            88,
            147,
            143,
            215,
            205,
            163,
            204,
            219,
            194,
            219,
            233,
            215,
            193,
            218,
            232,
            215,
            198,
            176,
            164,
            200,
            184,
            220,
            232,
            216,
            193,
            218,
            230,
            213,
            177,
            154,
            192,
            175,
            87,
            143,
            138,
            219,
            202,
            202,
            213,
            210,
            183,
            218,
            232,
            215,
            193,
            218,
            232,
            215,
            193,
            219,
            232,
            216,
            194,
            218,
            232,
            215,
            193,
            216,
            229,
            210,
            170,
            139,
            191,
            174,
            85,
            141,
            144,
            225,
            194,
            213,
            219,
            208,
            175,
            214,
            232,
            215,
            193,
            219,
            232,
            215,
            193,
            218,
            232,
            215,
            193,
            218,
            232,
            215,
            193,
            211,
            229,
            208,
            150,
            120,
            191,
            174,
            84,
            132,
            160,
            225,
            191,
            211,
            222,
            213,
            185,
            215,
            232,
            215,
            191,
            212,
            232,
            215,
            192,
            217,
            232,
            215,
            192,
            217,
            232,
            215,
            189,
            206,
            228,
            206,
            122,
            113,
            191,
            174,
            82,
            120,
            155,
            224,
            192,
            198,
            172,
            207,
            188,
            219,
            233,
            215,
            150,
            189,
            233,
            215,
            191,
            217,
            233,
            215,
            191,
            218,
            232,
            214,
            180,
            203,
            228,
            206,
            106,
            110,
            191,
            173,
            79,
            121,
            136,
            226,
            194,
            206,
            182,
            208,
            190,
            223,
            226,
            202,
            89,
            198,
            233,
            216,
            193,
            221,
            233,
            214,
            193,
            213,
            231,
            211,
            174,
            209,
            231,
            210,
            110,
            121,
            192,
            174,
            81,
            124,
            121,
            225,
            197,
            221,
            229,
            216,
            197,
            174,
            213,
            175,
            92,
            218,
            230,
            212,
            196,
            188,
            180,
            209,
            198,
            189,
            231,
            209,
            180,
            216,
            232,
            214,
            159,
            151,
            194,
            174,
            83,
            126,
            120,
            225,
            199,
            224,
            231,
            216,
            135,
            101,
            211,
            169,
            131,
            224,
            178,
            206,
            201,
            127,
            121,
            214,
            201,
            191,
            232,
            211,
            194,
            198,
            225,
            207,
            199,
            159,
            195,
            175,
            86,
            127,
            121,
            218,
            202,
            166,
            168,
            216,
            142,
            136,
            217,
            183,
            177,
            186,
            127,
            213,
            203,
            128,
            123,
            217,
            202,
            200,
            233,
            214,
            201,
            199,
            228,
            210,
            185,
            162,
            200,
            177,
            89,
            130,
            123,
            215,
            205,
            127,
            123,
            216,
            209,
            181,
            212,
            209,
            203,
            139,
            125,
            215,
            205,
            130,
            125,
            219,
            202,
            213,
            234,
            219,
            206,
            226,
            234,
            220,
            173,
            178,
            211,
            188,
            94,
            132,
            125,
            216,
            207,
            132,
            129,
            216,
            207,
            135,
            137,
            215,
            206,
            131,
            129,
            216,
            207,
            132,
            126,
            222,
            207,
            226,
            235,
            221,
            208,
            230,
            234,
            222,
            179,
            155,
            166,
            200,
            124,
            133,
            126,
            216,
            207,
            133,
            130,
            216,
            207,
            133,
            129,
            216,
            207,
            134,
            130,
            216,
            207,
            133,
            129,
            225,
            208,
            230,
            235,
            223,
            212,
            226,
            232,
            215,
            196,
            164,
            149,
            198,
            154,
            136,
            128,
            217,
            208,
            136,
            133,
            217,
            208,
            136,
            133,
            217,
            208,
            136,
            133,
            217,
            208,
            136,
            133,
            227,
            211,
            230,
            235,
            224,
            215,
            226,
            231,
            213,
            209,
            186,
            158,
            201,
            165,
            138,
            131,
            217,
            209,
            138,
            135,
            217,
            209,
            138,
            135,
            217,
            209,
            138,
            135,
            217,
            209,
            138,
            136,
            228,
            213,
            231,
            235,
            226,
            217,
            231,
            234,
            225,
            217,
            217,
            210,
            219,
            160,
            141,
            133,
            218,
            210,
            142,
            138,
            218,
            210,
            142,
            138,
            218,
            210,
            142,
            138,
            218,
            210,
            141,
            136,
            226,
            215,
            232,
            235,
            227,
            220,
            232,
            235,
            228,
            220,
            232,
            230,
            214,
            118,
            143,
            130,
            218,
            211,
            144,
            140,
            218,
            211,
            144,
            140,
            218,
            211,
            144,
            140,
            218,
            211,
            144,
            138,
            224,
            215,
            230,
            235,
            229,
            224,
            232,
            235,
            229,
            224,
            230,
            226,
            201,
            114,
            145,
            116,
            217,
            211,
            148,
            143,
            218,
            211,
            147,
            143,
            218,
            211,
            147,
            143,
            218,
            211,
            147,
            141,
            221,
            214,
            222,
            235,
            231,
            227,
            233,
            235,
            231,
            228,
            230,
            227,
            203,
            120,
            143,
            95,
            194,
            213,
            151,
            146,
            219,
            213,
            151,
            146,
            219,
            213,
            151,
            146,
            219,
            213,
            151,
            145,
            219,
            213,
            206,
            235,
            233,
            231,
            234,
            235,
            233,
            231,
            234,
            231,
            218,
            129,
            130,
            84,
            126,
            207,
            154,
            148,
            219,
            214,
            154,
            148,
            219,
            214,
            154,
            148,
            219,
            214,
            154,
            148,
            219,
            213,
            179,
            230,
            234,
            233,
            234,
            235,
            234,
            233,
            234,
            234,
            232,
            169,
            139,
            94,
            70,
            186,
            154,
            126,
            199,
            212,
            157,
            147,
            211,
            213,
            157,
            147,
            211,
            213,
            157,
            147,
            211,
            213,
            164,
            214,
            226,
            233,
            235,
            230,
            226,
            233,
            235,
            230,
            226,
            211
    };

    for (int i = 0; i < num_inputs / 2; i++) {
        mpq_set_ui(input_q[i], a[i], 1);
        mpq_set_ui(input_q[num_inputs / 2 + i], b[i], 1);
    }
}