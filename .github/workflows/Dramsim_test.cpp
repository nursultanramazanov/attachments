# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: dofile('Scripts/Database/wsTypes.lua')
dofile('Scripts/World/Radio/BeaconTypes.lua')
dofile('Scripts/World/Radio/BeaconSites.lua')

local disableNauticalBeacons = true

local gettext = require("i_18n")
local       _ = gettext.translate

--WORLD BEACONS

--[[
        Persian Gulf beacon mod v.1.0, updated August 5th, 2018.
        
        General info:
        I have added ICLS beacons for the F/A-18C for all runways that are already equipped with regular ILS.
        Instead of using DMEs co-located with the Glide Path(/Slope), I have added TACANS for use in aircraft without separate DME and VOR receivers.
        Three letter IDENT codes are used for TACAN, since the Hornet only displays the first three letters.
        All VORs (except Shiraz, which already has a TACAN) are converted to VORTACs, for use with both VOR and TACAN equiupped aircraft.
        
        Added beacons:
        - Al Ain: VORTAC ch. 73X / 112.6 MHz
        - Al Maktoum: VORTAC ch. 81 / 113.4 MHz
        - Al Maktoum RWY 12: ICLS ch. 8, TACAN ch. 54X
        - Al Maktoum RWY 30: ICLS ch. 12, TACAN ch. 35X
        - Al Minhad: VORTAC ch. 99 / 115.2 MHz
        - Al Minhad RWY 09: ICLS ch. 7, TACAN ch. 44X
        - Al Minhad RWY 27: ICLS ch. 9, TACAN ch. 45X
        - Bandar Abbas: VORTAC ch. 119 / 117.2 MHz
        - Bandar Abbas RWY21L: ICLS ch. 19, TACAN ch. 36X
        - Bandar Lengeh: VORTAC ch. 95 / 114.8 MHz
        - Dubai: VORTAC 21X / 108.4 MHz - new beacon located along the extended centerline of RWY 12L/30R
        - Dubai RWY 12L: ICLS ch. 11, TACAN ch. 38X
        - Dubai RWY 12R: ICLS ch. 15, TACAN ch. 32X
        - Dubai RWY 30L: ICLS ch. 13, TACAN ch. 50X
        - Dubai RWY 30R: ICLS ch. 19, TACAN ch. 46X
        - Fujairah: VORTAC ch. 85X
        - Fujairah RWY 29: ICLS ch. 1, TACAN ch. 40
        - Havadarya RWY 08: ICLS ch. 8, TACAN ch. 26X
        - Kerman: VORTAC ch. 57 / 112.0 MHz
        - Khasab RWY 19: ICLS ch. 3, TACAN ch. 40X
        - Kish Island: VORTAC ch. 121 / 117.4 MHz
        - Lar: VORTAC ch. 126X / 117.9 MHz
        - Lar RWY 09: ICLS ch. 15, TACAN ch. 52X
        - Qeshm Island: VORTAC ch. 118X / 117.1 MHz
        - Ras Al Khaimah: VORTAC ch. 83X / 113.6 MHz
        - Sharjah: VORTAC ch. 70X / 112.3 MHz
        - Sharjah RWY 12L: ICLS ch. 6, TACAN ch. 22X
        - Sharjah RWY 30L: ICLS ch. 20, TACAN ch. 56X
        - Sirri Island: VORTAC ch. 84Y / 113.75 MHz

        
]]--


beaconsTableFormat = 2
beacons = {
        {
                display_name = _('Abumusa');
                beaconId = 'airfield1_0';
                type = BEACON_TYPE_HOMER;
                callsign = 'ABM';
                frequency = 285000.000000;
                position = { -31340.871094, 5.000005, -122275.148438 };
                direction = 174.107027;
                positionGeo = { latitude = 25.877348, longitude = 55.023518 };
                sceneObjects = {'t:428933120'};
        };
        {
                display_name = _('BandarAbbas');
                beaconId = 'airfield2_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'BND';
                frequency = 117200000.000000;
                channel = 119;
                position = { 113480.484375, 5.854269, 13045.339844 };
                direction = 26.337865;
                positionGeo = { latitude = 27.197000, longitude = 56.366833 };
                sceneObjects = {'t:-1265303496'};
        };
        {
                display_name = _('BandarAbbas');
                beaconId = 'airfield2_1';
                type = BEACON_TYPE_HOMER;
                callsign = 'BND';
                frequency = 250000.000000;
                position = { 115560.210938, 16.835809, 12328.879883 };
                direction = 26.337865;
                positionGeo = { latitude = 27.215742, longitude = 56.359493 };
                sceneObjects = {'t:475070464'};
        };
        {
                display_name = _('');
                beaconId = 'airfield2_2';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IBND';
                frequency = 109900000.000000;
                position = { 113693.234375, 5.776140, 13155.006836 };
                direction = 28.001016;
                positionGeo = { latitude = 27.198926, longitude = 56.367930 };
                sceneObjects = {'t:197885952'};
                chartOffsetX = 4544.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield2_3';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IBND';
                frequency = 109900000.000000;
                position = { 116974.781250, 5.880138, 14981.854492 };
                direction = 26.337865;
                positionGeo = { latitude = 27.228633, longitude = 56.386212 };
                sceneObjects = {'t:-1265303495'};
        };
        {
                display_name = _('');
                beaconId = 'airfield2_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'BND';
                frequency = 15655000000.000000;
                channel = 19;
                position = { 113693.234375, 5.776140, 13155.006836 };
                direction = 28.001016;
                positionGeo = { latitude = 27.198926, longitude = 56.367930 };
                sceneObjects = {'t:197885952'};
                chartOffsetX = 4544.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield2_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'BND';
                frequency = 15655000000.000000;
                channel = 19;
                position = { 116974.781250, 5.880138, 14981.854492 };
                direction = 26.337865;
                positionGeo = { latitude = 27.228633, longitude = 56.386212 };
                sceneObjects = {'t:-1265303495'};
        };
        {
                display_name = _('');
                beaconId = 'airfield2_6';
                type = BEACON_TYPE_TACAN;
                callsign = 'BND';
                frequency = 109900000.000000;
                channel = 36;
                position = { 116974.781250, 5.880138, 14981.854492 };
                direction = 26.337865;
                positionGeo = { latitude = 27.228633, longitude = 56.386212 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _('BandarLengeh');
                beaconId = 'airfield3_0';
                type = BEACON_TYPE_HOMER;
                callsign = 'LEN';
                frequency = 408000.000000;
                position = { 41337.355469, 23.178969, -140284.187500 };
                direction = -1.600031;
                positionGeo = { latitude = 26.530575, longitude = 54.831754 };
                sceneObjects = {'t:435027968'};
        };
        {
                display_name = _('BandarLengeh');
                beaconId = 'airfield3_1';
                type = BEACON_TYPE_VORTAC;
                callsign = 'LEN';
                frequency = 114800000.000000;
                channel = 95;
                position = { 41944.980469, 20.168563, -138343.031250 };
                direction = -1.600031;
                positionGeo = { latitude = 26.536353, longitude = 54.851123 };
                sceneObjects = {'t:435060736'};
        };
        {
                display_name = _('AlDhafra');
                beaconId = 'airfield4_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'MA';
                frequency = 114900000.000000;
                channel = 96;
                position = { -211188.156250, 16.000016, -173477.796875 };
                direction = 37.615738;
                positionGeo = { latitude = 24.246803, longitude = 54.545387 };
                sceneObjects = {'t:335675392'};
        };
        {
                display_name = _('DUBAI');
                beaconId = 'airfield5_0';
                type = BEACON_TYPE_HOMER;
                callsign = 'DO';
                frequency = 265000.000000;
                position = { -103374.234375, 7.304522, -85496.226563 };
                direction = 30.557747;
                positionGeo = { latitude = 25.231649, longitude = 55.399134 };
                sceneObjects = {'t:402423808'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IDBW';
                frequency = 109500000.000000;
                position = { -103160.304688, 5.000005, -85509.226563 };
                direction = -57.960002;
                positionGeo = { latitude = 25.233579, longitude = 55.398979 };
                sceneObjects = {'t:-1265205182'};
                chartOffsetX = 5165.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_2';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IDBR';
                frequency = 110100000.000000;
                position = { -101655.835938, 5.000005, -87173.156250 };
                direction = -58.009998;
                positionGeo = { latitude = 25.246980, longitude = 55.382285 };
                sceneObjects = {'t:-1265205181'};
                chartOffsetX = 4461.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_3';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IDBE';
                frequency = 111300000.000000;
                position = { -100336.234375, 5.000005, -90012.007813 };
                direction = 122.059989;
                positionGeo = { latitude = 25.258581, longitude = 55.353947 };
                sceneObjects = {'t:-1265205180'};
                chartOffsetX = 5340.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_4';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IDBL';
                frequency = 110900000.000000;
                position = { -99354.195313, 5.000005, -90856.460938 };
                direction = 122.069989;
                positionGeo = { latitude = 25.267352, longitude = 55.345444 };
                sceneObjects = {'t:-1265205179'};
                chartOffsetX = 4326.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_5';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IDBL';
                frequency = 110900000.000000;
                position = { -101331.320313, 5.000005, -87516.156250 };
                direction = 123.104971;
                positionGeo = { latitude = 25.249872, longitude = 55.378842 };
                sceneObjects = {'t:-1265205178'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_6';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IDBR';
                frequency = 110100000.000000;
                position = { -99715.640625, 5.000005, -90031.437500 };
                direction = -60.241516;
                positionGeo = { latitude = 25.264181, longitude = 55.353679 };
                sceneObjects = {'t:-1265205177'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_7';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IDBE';
                frequency = 111300000.000000;
                position = { -102607.976563, 5.000005, -86169.898438 };
                direction = 124.706241;
                positionGeo = { latitude = 25.238494, longitude = 55.392357 };
                sceneObjects = {'t:-1265205176'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_8';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IDBW';
                frequency = 109500000.000000;
                position = { -101020.156250, 5.000005, -88667.859375 };
                direction = -57.330628;
                positionGeo = { latitude = 25.252555, longitude = 55.367372 };
                sceneObjects = {'t:-1265205175'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_9';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'DBW';
                frequency = 15535000000.000000;
                channel = 15;
                position = { -103160.304688, 5.000005, -85509.226563 };
                direction = -57.960002;
                positionGeo = { latitude = 25.233579, longitude = 55.398979 };
                sceneObjects = {'t:-1265205182'};
                chartOffsetX = 5165.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_10';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'DBR';
                frequency = 15415000000.000000;
                channel = 11;
                position = { -101655.835938, 5.000005, -87173.156250 };
                direction = -58.009998;
                positionGeo = { latitude = 25.246980, longitude = 55.382285 };
                sceneObjects = {'t:-1265205181'};
                chartOffsetX = 4461.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_11';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'DBE';
                frequency = 15475000000.000000;
                channel = 13;
                position = { -100336.234375, 5.000005, -90012.007813 };
                direction = 122.059989;
                positionGeo = { latitude = 25.258581, longitude = 55.353947 };
                sceneObjects = {'t:-1265205180'};
                chartOffsetX = 5340.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_12';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'DBL';
                frequency = 15655000000.000000;
                channel = 19;
                position = { -99354.195313, 5.000005, -90856.460938 };
                direction = 122.069989;
                positionGeo = { latitude = 25.267352, longitude = 55.345444 };
                sceneObjects = {'t:-1265205179'};
                chartOffsetX = 4326.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield5_13';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'DBL';
                frequency = 15655000000.000000;
                channel = 19;
                position = { -101331.320313, 5.000005, -87516.156250 };
                direction = 123.104971;
                positionGeo = { latitude = 25.249872, longitude = 55.378842 };
                sceneObjects = {'t:-1265205178'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_14';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'DBR';
                frequency = 15415000000.000000;
                channel = 11;
                position = { -99715.640625, 5.000005, -90031.437500 };
                direction = -60.241516;
                positionGeo = { latitude = 25.264181, longitude = 55.353679 };
                sceneObjects = {'t:-1265205177'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_15';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'DBE';
                frequency = 15475000000.000000;
                channel = 13;
                position = { -102607.976563, 5.000005, -86169.898438 };
                direction = 124.706241;
                positionGeo = { latitude = 25.238494, longitude = 55.392357 };
                sceneObjects = {'t:-1265205176'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_16';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'DBW';
                frequency = 15535000000.000000;
                channel = 15;
                position = { -101020.156250, 5.000005, -88667.859375 };
                direction = -57.330628;
                positionGeo = { latitude = 25.252555, longitude = 55.367372 };
                sceneObjects = {'t:-1265205175'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_17';
                type = BEACON_TYPE_TACAN;
                callsign = 'DBL';
                frequency = 110900000.000000;
                channel = 46;
                position = { -101331.320313, 5.000005, -87516.156250 };
                direction = 123.104971;
                positionGeo = { latitude = 25.249872, longitude = 55.378842 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _('');
                beaconId = 'airfield5_18';
                type = BEACON_TYPE_TACAN;
                callsign = 'DBR';
                frequency = 110100000.000000;
                channel = 38;
                position = { -99715.640625, 5.000005, -90031.437500 };
                direction = -60.241516;
                positionGeo = { latitude = 25.264181, longitude = 55.353679 };
                sceneObjects = {'t:-1265074090'};
        };

        {
                display_name = _('');
                beaconId = 'airfield5_19';
                type = BEACON_TYPE_TACAN;
                callsign = 'DBE';
                frequency = 111300000.000000;
                channel = 50;
                position = { -102607.976563, 5.000005, -86169.898438 };
                direction = 124.706241;
                positionGeo = { latitude = 25.238494, longitude = 55.392357 };                
                sceneObjects = {'t:-1265074090'};
        };

        {
                display_name = _('');
                beaconId = 'airfield5_20';
                type = BEACON_TYPE_TACAN;
                callsign = 'DBW';
                frequency = 109500000.000000;
                channel = 32;
                position = { -101020.156250, 5.000005, -88667.859375 };
                direction = -57.330628;
                positionGeo = { latitude = 25.252555, longitude = 55.367372 };
                sceneObjects = {'t:-1265074090'};
        };

        {
                display_name = _('');
                beaconId = 'airfield5_21';
                type = BEACON_TYPE_VORTAC;
                callsign = 'DXB';
                frequency = 108400000.000000;
                channel = 21;
                position = { -102120, 5, -86442 };
                direction = 37.615738;
                positionGeo = { latitude = 25.242767, longitude = 55.389433 };
                sceneObjects = {'t:335675392'};
        };
        {
                display_name = _('AlMaktoumIntl');
                beaconId = 'airfield6_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'JXB';
                frequency = 113400000.000000;
                channel = 81;
                position = { -142027.656250, 37.618499, -107028.085938 };
                direction = -167.121177;
                positionGeo = { latitude = 24.880274, longitude = 55.190583 };
                sceneObjects = {'t:379257130'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IJEA';
                frequency = 111750000.000000;
                position = { -141530.671875, 37.618499, -107812.914063 };
                direction = -58.079994;
                positionGeo = { latitude = 24.884666, longitude = 55.182751 };
                sceneObjects = {'t:-1265172389'};
                chartOffsetX = 5249.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IJWA';
                frequency = 109750000.000000;
                position = { -141086.531250, 37.618499, -108299.953125 };
                direction = -56.676742;
                positionGeo = { latitude = 24.888616, longitude = 55.177872 };
                sceneObjects = {'t:380174336'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_3';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IJEA';
                frequency = 111750000.000000;
                position = { -139039.453125, 37.618499, -111508.414063 };
                direction = 121.722933;
                positionGeo = { latitude = 24.906703, longitude = 55.145846 };
                sceneObjects = {'t:-1265172387'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_4';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IJWA';
                frequency = 109750000.000000;
                position = { -138832.656250, 37.618499, -112149.820313 };
                direction = 121.949994;
                positionGeo = { latitude = 24.908491, longitude = 55.139470 };
                sceneObjects = {'t:-1265172388'};
                chartOffsetX = 5248.000000;
        };
        {
                display_name = _(''); -- RWY 12
                beaconId = 'airfield6_5';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'JEA';
                frequency = 15625000000.000000;
                channel = 8;
                position = { -141530.671875, 37.618499, -107812.914063 };
                direction = -58.079994;
                positionGeo = { latitude = 24.884666, longitude = 55.182751 };
                sceneObjects = {'t:-1265172389'};
                chartOffsetX = 5249.000000;
        };
        {
                display_name = _(''); -- RWY 30
                beaconId = 'airfield6_6';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'JWA';
                frequency = 15445000000.000000;
                channel = 12;
                position = { -141086.531250, 37.618499, -108299.953125 };
                direction = 121.722933;
                positionGeo = { latitude = 24.888616, longitude = 55.177872 };
                sceneObjects = {'t:380174336'};
        };
        {
                display_name = _(''); -- RWY 12
                beaconId = 'airfield6_7';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'JEA';
                frequency = 15625000000.000000;
                channel = 8;
                position = { -139039.453125, 37.618499, -111508.414063 };
                direction = -56.676742;
                positionGeo = { latitude = 24.906703, longitude = 55.145846 };
                sceneObjects = {'t:-1265172387'};
        };
        {
                display_name = _(''); -- RWY 30
                beaconId = 'airfield6_8';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'JWA';
                frequency = 15445000000.000000;
                channel = 12;
                position = { -138832.656250, 37.618499, -112149.820313 };
                direction = 121.949994;
                positionGeo = { latitude = 24.908491, longitude = 55.139470 };
                sceneObjects = {'t:-1265172388'};
                chartOffsetX = 5248.000000;
        };
        {
                display_name = _(''); -- RWY 30
                beaconId = 'airfield6_9';
                type = BEACON_TYPE_TACAN;
                callsign = 'JWA';
                frequency = 109800000.000000;
                channel = 35;
                position = { -141086.531250, 37.618499, -108299.953125 };
                direction = -56.676742;
                positionGeo = { latitude = 24.888616, longitude = 55.177872 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _(''); -- RWY 12
                beaconId = 'airfield6_10';
                type = BEACON_TYPE_TACAN;
                callsign = 'JEA';
                frequency = 111700000.000000;
                channel = 54;
                position = { -139039.453125, 37.618499, -111508.414063 };
                direction = 121.722933;
                positionGeo = { latitude = 24.906703, longitude = 55.145846 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _('Fujairah');
                beaconId = 'airfield7_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'FJV';
                frequency = 113800000.000000;
                channel = 85;
                position = { -118684.765625, 3.201513, 10676.752930 };
                direction = -157.881708;
                positionGeo = { latitude = 25.100651, longitude = 56.354556 };
                sceneObjects = {'t:393838649'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_1';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IFJR';
                frequency = 111100000.000000;
                position = { -118135.539063, 19.503962, 9027.303711 };
                direction = 112.627742;
                positionGeo = { latitude = 25.105538, longitude = 56.338172 };
                sceneObjects = {'t:-1265139689'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_2';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IFJR';
                frequency = 111100000.000000;
                position = { -116548.843750, 44.992561, 5567.033203 };
                direction = 112.589986;
                positionGeo = { latitude = 25.119710, longitude = 56.303773 };
                sceneObjects = {'t:-1265139690'};
                chartOffsetX = 4201.000000;
        };
                {
                display_name = _('');
                beaconId = 'airfield7_3';
                type = BEACON_TYPE_TACAN;
                callsign = 'FJR';
                frequency = 110300000.000000;
                channel = 40;
                position = { -118135.539063, 19.503962, 9027.303711 };
                direction = 112.627742;
                positionGeo = { latitude = 25.105538, longitude = 56.338172 };
                sceneObjects = {'t:-1265041393'};
        };        
        {
                display_name = _('');
                beaconId = 'airfield7_4';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'FJR';
                frequency = 15415000000.000000;
                channel = 1;
                position = { -118135.539063, 19.503962, 9027.303711 };
                direction = 112.627742;
                positionGeo = { latitude = 25.105538, longitude = 56.338172 };
                sceneObjects = {'t:-1265139689'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_5';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'FJR';
                frequency = 15415000000.000000;
                channel = 1;
                position = { -116548.843750, 44.992561, 5567.033203 };
                direction = 112.589986;
                positionGeo = { latitude = 25.119710, longitude = 56.303773 };
                sceneObjects = {'t:-1265139690'};
                chartOffsetX = 4201.000000;
        };
        {
                display_name = _('Havadarya');
                beaconId = 'airfield9_0';
                type = BEACON_TYPE_TACAN;
                callsign = 'HDR';
                frequency = 111000000.000000;
                channel = 47;
                position = { 109557.203125, 11.012902, -6187.317383 };
                direction = -130.596154;
                positionGeo = { latitude = 27.160571, longitude = 56.172924 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _('');
                beaconId = 'airfield9_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IBHD';
                frequency = 108900000.000000;
                position = { 109569.265625, 9.455730, -4889.315430 };
                direction = -100.999995;
                positionGeo = { latitude = 27.160757, longitude = 56.186023 };
                sceneObjects = {'t:-1265074128'};
                chartOffsetX = 2717.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield9_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IBHD';
                frequency = 108900000.000000;
                position = { 109172.148438, 6.802046, -7163.755371 };
                direction = -100.911496;
                positionGeo = { latitude = 27.157037, longitude = 56.163095 };
                sceneObjects = {'t:-1265074127'};
        };
        {
                display_name = _(''); -- RWY 09
                beaconId = 'airfield9_3';
                type = BEACON_TYPE_TACAN;
                callsign = 'BHD';
                frequency = 108900000.000000;
                channel = 26;
                position = { 109172.148438, 6.802046, -7163.755371 };
                direction = -100.911496;
                positionGeo = { latitude = 27.157037, longitude = 56.163095 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _(''); -- RWY 09
                beaconId = 'airfield9_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'BHD';
                frequency = 15625000000.000000;
                channel = 8;
                position = { 109569.265625, 9.455730, -4889.315430 };
                direction = -100.999995;
                positionGeo = { latitude = 27.160757, longitude = 56.186023 };
                sceneObjects = {'t:-1265074128'};
                chartOffsetX = 2717.000000;
        };
        {
                display_name = _(''); -- RWY 09
                beaconId = 'airfield9_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'BHD';
                frequency = 15625000000.000000;
                channel = 8;
                position = { 109172.148438, 6.802046, -7163.755371 };
                direction = -100.911496;
                positionGeo = { latitude = 27.157037, longitude = 56.163095 };
                sceneObjects = {'t:-1265074127'};
        };
        {
                display_name = _('KERMAN');
                beaconId = 'airfield18_0';
                type = BEACON_TYPE_HOMER;
                callsign = 'KER';
                frequency = 290000.000000;
                position = { 453186.718750, 1751.425580, 71843.406250 };
                direction = 144.697285;
                positionGeo = { latitude = 30.264720, longitude = 56.959325 };
                sceneObjects = {'t:133792405'};
        };
        {
                display_name = _('KERMAN');
                beaconId = 'airfield18_1';
                type = BEACON_TYPE_VORTAC;
                callsign = 'KER';
                frequency = 112000000.000000;
                channel = 57;
                position = { 455450.843750, 1751.425824, 70352.039063 };
                direction = 0.000000;
                positionGeo = { latitude = 30.285146, longitude = 56.943809 };
                sceneObjects = {'t:304185408'};
        };
        {
                display_name = _('Khasab');
                beaconId = 'airfield10_0';
                type = BEACON_TYPE_TACAN;
                callsign = 'KSB';
                frequency = 113700000.000000;
                channel = 84;
                position = { 667.781860, 15.763658, -91.611923 };
                direction = 88.473189;
                positionGeo = { latitude = 26.177844, longitude = 56.240980 };
                sceneObjects = {'t:-1265041393'};
        };
        {
                display_name = _('');
                beaconId = 'airfield10_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IBKS';
                frequency = 110300000.000000;
                position = { -1521.258057, 31.899736, -503.110107 };
                direction = 14.030001;
                positionGeo = { latitude = 26.158057, longitude = 56.236991 };
                sceneObjects = {'t:-1265041400'};
                chartOffsetX = 2627.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield10_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IBKS';
                frequency = 110300000.000000;
                position = { 673.278259, 15.972951, 101.777466 };
                direction = 14.261517;
                positionGeo = { latitude = 26.177903, longitude = 56.242914 };
                sceneObjects = {'t:-1265041399'};
                chartOffsetX = 1000.000000;
        };
                {
                display_name = _('');
                beaconId = 'airfield10_3';
                type = BEACON_TYPE_TACAN;
                callsign = 'BKS';
                frequency = 110300000.000000;
                channel = 40;
                position = { 673.278259, 15.972951, 101.777466 };
                direction = 14.261517;
                positionGeo = { latitude = 26.177903, longitude = 56.242914 };
                sceneObjects = {'t:-1265041393'};
        };
        {
                display_name = _('');
                beaconId = 'airfield10_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'BKS';
                frequency = 15475000000.000000;
                channel = 3;
                position = { -1521.258057, 31.899736, -503.110107 };
                direction = 14.030001;
                positionGeo = { latitude = 26.158057, longitude = 56.236991 };
                sceneObjects = {'t:-1265041400'};
                chartOffsetX = 2627.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield10_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'BKS';
                frequency = 15475000000.000000;
                channel = 3;
                position = { 673.278259, 15.972951, 101.777466 };
                direction = 14.261517;
                positionGeo = { latitude = 26.177903, longitude = 56.242914 };
                sceneObjects = {'t:-1265041399'};
                chartOffsetX = 1000.000000;
        };
        {
                display_name = _('LAR');
                beaconId = 'airfield11_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'LAR';
                frequency = 117900000.000000;
                channel = 126;
                position = { 168932.671875, 792.860046, -179095.390625 };
                direction = 0.000000;
                positionGeo = { latitude = 27.674948, longitude = 54.416221 };
                sceneObjects = {'t:515735552'};
        };
        {
                display_name = _('');
                beaconId = 'airfield11_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'OISL';
                frequency = 111500000.000000;
                position = { 168963.328125, 798.975591, -180089.718750 };
                direction = -89.651007;
                positionGeo = { latitude = 27.675037, longitude = 54.406142 };
                sceneObjects = {'t:216399872'};
                chartOffsetX = 3885.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield11_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'OISL';
                frequency = 111500000.000000;
                position = { 168903.421875, 803.328928, -183491.234375 };
                direction = -88.808462;
                positionGeo = { latitude = 27.673847, longitude = 54.371698 };
                sceneObjects = {'t:515670094'};
        };
        {
                display_name = _('LAR');
                beaconId = 'airfield11_3';
                type = BEACON_TYPE_HOMER;
                callsign = 'OISL';
                frequency = 224000.000000;
                position = { 168452.046875, 803.328928, -183086.562500 };
                direction = 0.000000;
                positionGeo = { latitude = 27.669854, longitude = 54.375895 };
                sceneObjects = {'t:515670098'};
        };
        {
                display_name = _('');
                beaconId = 'airfield11_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'ISL';
                frequency = 15535000000.000000;
                channel = 15;
                position = { 168963.328125, 798.975591, -180089.718750 };
                direction = -89.651007;
                positionGeo = { latitude = 27.675037, longitude = 54.406142 };
                sceneObjects = {'t:216399872'};
                chartOffsetX = 3885.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield11_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'ISL';
                frequency = 15535000000.000000;
                channel = 15;
                position = { 168903.421875, 803.328928, -183491.234375 };
                direction = -88.808462;
                positionGeo = { latitude = 27.673847, longitude = 54.371698 };
                sceneObjects = {'t:515670094'};
        };
        {
                display_name = _('');
                beaconId = 'airfield11_6';
                type = BEACON_TYPE_TACAN;
                callsign = 'ISL';
                frequency = 111500000.000000;
                channel = 52;
                position = { 168903.421875, 803.328928, -183491.234375 };
                direction = -88.808462;
                positionGeo = { latitude = 27.673847, longitude = 54.371698 };
                sceneObjects = {'t:-1265074090'};
        };
        {
                display_name = _('Minhad');
                beaconId = 'airfield12_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'MIN';
                frequency = 115200000.000000;
                channel = 99;
                position = { -126012.203125, 56.568256, -86126.000000 };
                direction = -90.145369;
                positionGeo = { latitude = 25.027220, longitude = 55.395556 };
                sceneObjects = {'t:-1265008566'};
        };
        {
                display_name = _('');
                beaconId = 'airfield12_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IMNW';
                frequency = 110700000.000000;
                position = { -126014.757813, 57.834752, -86793.367188 };
                direction = -90.009988;
                positionGeo = { latitude = 25.027125, longitude = 55.388944 };
                sceneObjects = {'t:-1265008565'};
                chartOffsetX = 4567.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield12_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IMNW';
                frequency = 110700000.000000;
                position = { -125893.375000, 52.421538, -90741.046875 };
                direction = -90.145369;
                positionGeo = { latitude = 25.027792, longitude = 55.349815 };
                sceneObjects = {'t:-1265008563'};
        };
        {
                display_name = _('');
                beaconId = 'airfield12_3';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IMNR';
                frequency = 110750000.000000;
                position = { -126015.968750, 52.421538, -91371.843750 };
                direction = 89.980011;
                positionGeo = { latitude = 25.026616, longitude = 55.343580 };
                sceneObjects = {'t:-1265008564'};
                chartOffsetX = 4549.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield12_4';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IMNR';
                frequency = 110750000.000000;
                position = { -125895.132813, 58.193624, -87437.218750 };
                direction = 90.000474;
                positionGeo = { latitude = 25.028136, longitude = 55.382550 };
                sceneObjects = {'t:-1265008562'};
        };
        {
                display_name = _(''); -- rwy 09
                beaconId = 'airfield12_5';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'MNW';
                frequency = 15595000000.000000;
                channel = 7;
                position = { -126014.757813, 57.834752, -86793.367188 };
                direction = -90.009988;
                positionGeo = { latitude = 25.027125, longitude = 55.388944 };
                sceneObjects = {'t:-1265008565'};
                chartOffsetX = 4567.000000;
        };
        {
                display_name = _(''); -- rwy 09
                beaconId = 'airfield12_6';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'MNW';
                frequency = 15595000000.000000;
                channel = 7;
                position = { -125893.375000, 52.421538, -90741.046875 };
                direction = -90.145369;
                positionGeo = { latitude = 25.027792, longitude = 55.349815 };
                sceneObjects = {'t:-1265008563'};
        };
        {
                display_name = _(''); -- rwy 27
                beaconId = 'airfield12_7';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'MNR';
                frequency = 15655000000.000000;
                channel = 9;
                position = { -126015.968750, 52.421538, -91371.843750 };
                direction = 89.980011;
                positionGeo = { latitude = 25.026616, longitude = 55.343580 };
                sceneObjects = {'t:-1265008564'};
                chartOffsetX = 4549.000000;
        };
        {
                display_name = _(''); -- rwy 27
                beaconId = 'airfield12_8';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'MNR';
                frequency = 15655000000.000000;
                channel = 9;
                position = { -125895.132813, 58.193624, -87437.218750 };
                direction = 90.000474;
                positionGeo = { latitude = 25.028136, longitude = 55.382550 };
                sceneObjects = {'t:-1265008562'};
        };
        {
                display_name = _(''); -- rwy 09
                beaconId = 'airfield12_9';
                type = BEACON_TYPE_TACAN;
                callsign = 'MNW';
                frequency = 110700000.000000;
                channel = 44;
                position = { -125893.375000, 52.421538, -90741.046875 };
                direction = -90.145369;
                positionGeo = { latitude = 25.027792, longitude = 55.349815 };
                sceneObjects = {'t:-1265008563'};
        };
        {
                display_name = _(''); -- rwy 27
                beaconId = 'airfield12_10';
                type = BEACON_TYPE_TACAN;
                callsign = 'MNR';
                frequency = 110800000.000000;
                channel = 45;
                position = { -125895.132813, 58.193624, -87437.218750 };
                direction = 90.000474;
                positionGeo = { latitude = 25.028136, longitude = 55.382550 };
                sceneObjects = {'t:-1265008562'};
        };
        {
                display_name = _('GheshmIsland');
                beaconId = 'airfield13_0';
                type = BEACON_TYPE_HOMER;
                callsign = 'KHM';
                frequency = 233000.000000;
                position = { 65696.914063, 5.999469, -32833.363281 };
                direction = 141.512093;
                positionGeo = { latitude = 26.762802, longitude = 55.907839 };
                sceneObjects = {'t:443514880'};
        };
        {
                display_name = _('GheshmIsland');
                beaconId = 'airfield13_1';
                type = BEACON_TYPE_VORTAC;
                callsign = 'KHM';
                frequency = 117100000.000000;
                channel = 118;
                position = { 64811.847656, 5.999469, -33663.304688 };
                direction = 0.000000;
                positionGeo = { latitude = 26.754748, longitude = 55.899569 };
                sceneObjects = {'t:78282784'};
        };
        {
                display_name = _('');
                beaconId = 'airfield14_0';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'ISRE';
                frequency = 108550000.000000;
                position = { -93834.992188, 29.999980, -71385.187500 };
                direction = -57.030006;
                positionGeo = { latitude = 25.319219, longitude = 55.538154 };
                sceneObjects = {'t:-1264943044'};
                chartOffsetX = 4492.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield14_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'ISHW';
                frequency = 111950000.000000;
                position = { -91290.867188, 29.999980, -75307.093750 };
                direction = 122.970016;
                positionGeo = { latitude = 25.341794, longitude = 55.498918 };
                sceneObjects = {'t:-1264943043'};
                chartOffsetX = 4480.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield14_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'ISHW';
                frequency = 111950000.000000;
                position = { -93187.609375, 29.972640, -72018.781250 };
                direction = 121.869334;
                positionGeo = { latitude = 25.325000, longitude = 55.531791 };
                sceneObjects = {'t:-1264943042'};
        };
        {
                display_name = _('');
                beaconId = 'airfield14_3';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'ISRE';
                frequency = 108550000.000000;
                position = { -91604.445313, 29.999980, -74424.015625 };
                direction = -57.111762;
                positionGeo = { latitude = 25.339053, longitude = 55.507725 };
                sceneObjects = {'t:-1264943041'};
        };
        {
                display_name = _('Sharjah');  
                beaconId = 'airfield14_4';
                type = BEACON_TYPE_VORTAC;
                callsign = 'SHJ';
                frequency = 112300000.000000;
                channel = 70;
                position = { -92371.390625, 28.209940, -73056.312500 };
                direction = 122.888246;
                positionGeo = { latitude = 25.332266, longitude = 55.521396 };
                sceneObjects = {'t:408617017'};
        };
                {
                display_name = _(''); -- 12L
                beaconId = 'airfield14_5';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'SRE';
                frequency = 15565000000.000000;
                channel = 6;
                position = { -93834.992188, 29.999980, -71385.187500 };
                direction = -57.030006;
                positionGeo = { latitude = 25.319219, longitude = 55.538154 };
                sceneObjects = {'t:-1264943044'};
                chartOffsetX = 4492.000000;
        };
        {
                display_name = _(''); -- 30R
                beaconId = 'airfield14_6';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'SHW';
                frequency = 15685000000.000000;
                channel = 20;
                position = { -91290.867188, 29.999980, -75307.093750 };
                direction = 122.970016;
                positionGeo = { latitude = 25.341794, longitude = 55.498918 };
                sceneObjects = {'t:-1264943043'};
                chartOffsetX = 4480.000000;
        };
        {
                display_name = _(''); -- 30R
                beaconId = 'airfield14_7';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'SHW';
                frequency = 15685000000.000000;
                channel = 20;
                position = { -93187.609375, 29.972640, -72018.781250 };
                direction = 121.869334;
                positionGeo = { latitude = 25.325000, longitude = 55.531791 };
                sceneObjects = {'t:-1264943042'};
        };
        {
                display_name = _(''); -- 12L
                beaconId = 'airfield14_8';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'SRE';
                frequency = 15565000000.000000;
                channel = 6;
                position = { -91604.445313, 29.999980, -74424.015625 };
                direction = -57.111762;
                positionGeo = { latitude = 25.339053, longitude = 55.507725 };
                sceneObjects = {'t:-1264943041'};
        };
                {
                display_name = _(''); -- rwy 12L
                beaconId = 'airfield14_9';
                type = BEACON_TYPE_TACAN;
                callsign = 'SRE';
                frequency = 108550000.000000;
                channel = 22;
                position = { -91604.445313, 29.999980, -74424.015625 };
                direction = -57.111762;
                positionGeo = { latitude = 25.339053, longitude = 55.507725 };
                sceneObjects = {'t:-1265008562'};
        };
        {
                display_name = _(''); -- rwy 30R
                beaconId = 'airfield14_10';
                type = BEACON_TYPE_TACAN;
                callsign = 'SHW';
                frequency = 111900000.000000;
                channel = 56;
                position = { -93187.609375, 29.972640, -72018.781250 };
                direction = 121.869334;
                positionGeo = { latitude = 25.325000, longitude = 55.531791 };
                sceneObjects = {'t:-1265008562'};
        };
        {
                display_name = _('SHIRAZ');
                beaconId = 'airfield19_0';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'SYZ';
                frequency = 117800000.000000;
                channel = 125;
                position = { 381030.062500, 1487.001487, -351865.593750 };
                direction = -153.938912;
                positionGeo = { latitude = 29.540193, longitude = 52.588744 };
                sceneObjects = {'t:122491860'};
        };
        {
                display_name = _('SHIRAZ');
                beaconId = 'airfield19_1';
                type = BEACON_TYPE_TACAN;
                callsign = 'SYZ1';
                frequency = 114700000.000000;
                channel = 94;
                position = { 381238.562500, 1487.001487, -352249.281250 };
                direction = -153.938912;
                positionGeo = { latitude = 29.541938, longitude = 52.584714 };
                sceneObjects = {'t:651165847'};
        };
        {
                display_name = _('SHIRAZ');
                beaconId = 'airfield19_2';
                type = BEACON_TYPE_HOMER;
                callsign = 'SR';
                frequency = 205000.000000;
                position = { 379775.937500, 1487.001365, -350896.250000 };
                direction = 0.000000;
                positionGeo = { latitude = 29.529239, longitude = 52.599209 };
                sceneObjects = {'t:649756715'};
        };
        {
                display_name = _('SirriIsland');
                beaconId = 'airfield15_0';
                type = BEACON_TYPE_HOMER;
                callsign = 'SIR';
                frequency = 300000.000000;
                position = { -27844.582031, 5.338042, -169988.296875 };
                direction = 38.778616;
                positionGeo = { latitude = 25.901625, longitude = 54.546971 };
                sceneObjects = {'t:75893228'};
        };
        {
                display_name = _('SirriIsland');
                beaconId = 'airfield15_1';
                type = BEACON_TYPE_VORTAC;
                callsign = 'SIR';
                frequency = 113750000.000000;
                channel = 84;
                position = { -26408.787109, 8.874265, -171201.140625 };
                direction = 0.000000;
                positionGeo = { latitude = 25.914373, longitude = 54.534604 };
                sceneObjects = {'t:75890968'};
        };
        {
                display_name = _('Kochak');
                beaconId = 'airfield16_0';
                type = BEACON_TYPE_TACAN;
                callsign = 'KCK';
                frequency = 114200000.000000;
                channel = 89;
                position = { 8918.999023, 4.131644, -109438.015625 };
                direction = -7.639437;
                positionGeo = { latitude = 26.242384, longitude = 55.145874 };
                sceneObjects = {'t:182976541'};
        };
        {
                display_name = _('Kish');
                beaconId = 'world_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'KIS';
                frequency = 117400000.000000;
                channel = 121;
                position = { 42498.050781, 20.856592, -226966.140625 };
                direction = 0.000000;
                positionGeo = { latitude = 26.525169, longitude = 53.962352 };
                sceneObjects = {'t:77103104'};
        };
        {
                display_name = _('AlAin');
                beaconId = 'world_1';
                type = BEACON_TYPE_VORTAC;
                callsign = 'ALN';
                frequency = 112600000.000000;
                channel = 73;
                position = { -211213.640625, 243.331756, -65684.554688 };
                direction = 0.000000;
                positionGeo = { latitude = 24.260000, longitude = 55.606667 };
                sceneObjects = {'t:57704455'};
        };
        {
                display_name = _('RasAlKhaiman');
                beaconId = 'world_2';
                type = BEACON_TYPE_VORTAC;
                callsign = 'RAV';
                frequency = 113600000.000000;
                channel = 83;
                position = { -64417.347656, 30.405420, -30027.595703 };
                direction = 0.000000;
                positionGeo = { latitude = 25.588333, longitude = 55.946667 };
                sceneObjects = {'t:73860331'};
        };
}
  push: dofile('Scripts/Database/wsTypes.lua')
dofile('Scripts/World/Radio/BeaconTypes.lua')
dofile('Scripts/World/Radio/BeaconSites.lua')

local disableNauticalBeacons = true

local gettext = require("i_18n")
local       _ = gettext.translate

--WORLD BEACONS

beaconsTableFormat = 2
beacons = {
        {
                display_name = _('DEIR_EZ-ZOR');
                beaconId = 'world_0';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'DRZ';
                frequency = 295000.000000;
                position = { 25885.554688, 210.741055, 390774.875000 };
                direction = 0.000000;
                positionGeo = { latitude = 35.289111, longitude = 40.187417 };
                sceneObjects = {'t:265781248'};
        };
        {
                display_name = _('GAZIANTEP');
                beaconId = 'world_1';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'GAZ';
                frequency = 432000.000000;
                position = { 210917.500000, 692.497213, 146450.375000 };
                direction = 0.000000;
                positionGeo = { latitude = 36.953265, longitude = 37.468546 };
                sceneObjects = {'t:348651520'};
        };
        {
                display_name = _('BANIAS');
                beaconId = 'world_2';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'BAN';
                frequency = 304000.000000;
                position = { 22735.679688, 8.349090, 5935.380859 };
                direction = 0.000000;
                positionGeo = { latitude = 35.228286, longitude = 35.957919 };
                sceneObjects = {'t:263324964'};
        };
        {
                display_name = _('ALEPPO');
                beaconId = 'world_3';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'ALE';
                frequency = 396000.000000;
                position = { 126175.296875, 383.767260, 123040.015625 };
                direction = 0.000000;
                positionGeo = { latitude = 36.185997, longitude = 37.223297 };
                sceneObjects = {'t:310182023'};
        };
        {
                display_name = _('KAHRAMANMARAS');
                beaconId = 'world_4';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'KHM';
                frequency = 374000.000000;
                position = { 276904.968750, 501.965376, 101895.742188 };
                direction = 0.000000;
                positionGeo = { latitude = 37.540196, longitude = 36.952346 };
                sceneObjects = {'t:374571008'};
        };
        {
                display_name = _('MEZZEH');
                beaconId = 'world_5';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'MEZ';
                frequency = 358000.000000;
                position = { -171265.828125, 727.153620, 25122.662109 };
                direction = 0.000000;
                positionGeo = { latitude = 33.485917, longitude = 36.226722 };
                sceneObjects = {'t:201690543'};
        };
        {
                display_name = _('KLEYATE');
                beaconId = 'world_6';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'RA';
                frequency = 450000.000000;
                position = { -48636.152344, 4.410843, 7884.588867 };
                direction = 0.000000;
                positionGeo = { latitude = 34.586111, longitude = 36.002778 };
                sceneObjects = {'t:243567680'};
        };
        {
                display_name = _('KARIATAIN');
                beaconId = 'world_7';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'KTN';
                frequency = 372500.000000;
                position = { -91109.367188, 752.277974, 120109.953125 };
                direction = 0.000000;
                positionGeo = { latitude = 34.227181, longitude = 37.233667 };
                sceneObjects = {'t:232622590'};
        };
        {
                display_name = _('ALEPPO');
                beaconId = 'world_8';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'MER';
                frequency = 365000.000000;
                position = { 124610.679688, 372.728827, 130592.070313 };
                direction = 0.000000;
                positionGeo = { latitude = 36.173114, longitude = 37.307553 };
                sceneObjects = {'t:310249747'};
        };
        {
                display_name = _('TURAIF');
                beaconId = 'world_9';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'TRF';
                frequency = 116100000.000000;
                position = { -373082.937500, 851.637692, 257743.343750 };
                direction = 0.000000;
                positionGeo = { latitude = 31.696111, longitude = 38.735583 };
                sceneObjects = {'t:44367872'};
        };
        {
                display_name = _('DEIR_EZ-ZOR');
                beaconId = 'world_10';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'DRZ';
                frequency = 117000000.000000;
                position = { 26163.650391, 221.187477, 387712.218750 };
                direction = 0.000000;
                positionGeo = { latitude = 35.291944, longitude = 40.153778 };
                sceneObjects = {'t:91979776'};
        };
        {
                display_name = _('BAYSUR');
                beaconId = 'world_11';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'BAR';
                frequency = 113900000.000000;
                position = { -137939.109375, 834.106852, -36216.691406 };
                direction = 0.000000;
                positionGeo = { latitude = 33.769444, longitude = 35.555556 };
                sceneObjects = {'t:73436275'};
        };
        {
                display_name = _('ALEPPO');
                beaconId = 'world_12';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'ALE';
                frequency = 114500000.000000;
                position = { 125445.273736, 382.200455, 121782.458582 };
                direction = 0.000000;
                positionGeo = { latitude = 36.179210, longitude = 37.209468 };
                sceneObjects = {'t:106172250'};
        };
        {
                display_name = _('MARKA');
                beaconId = 'world_13';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'AMN';
                frequency = 116300000.000000;
                position = { -335215.781250, 672.064637, 5626.460938 };
                direction = 0.000000;
                positionGeo = { latitude = 32.004072, longitude = 36.065986 };
                sceneObjects = {'t:44400927'};
        };
        {
                display_name = _('GAZIANTEP');
                beaconId = 'world_14';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'GAZ';
                frequency = 116700000.000000;
                position = { 210721.703125, 688.248064, 146838.281250 };
                direction = 0.000000;
                positionGeo = { latitude = 36.951556, longitude = 37.472937 };
                sceneObjects = {'t:121798656'};
        };
        {
                display_name = _('ROSH-PINA');
                beaconId = 'world_15';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'ROP';
                frequency = 115300000.000000;
                position = { -225297.281250, 261.793353, -37497.257813 };
                direction = 0.000000;
                positionGeo = { latitude = 32.982528, longitude = 35.572778 };
                sceneObjects = {'t:59769246'};
        };
        {
                display_name = _('TANF');
                beaconId = 'world_16';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'TAN';
                frequency = 114000000.000000;
                position = { -175046.437500, 704.918491, 250662.015625 };
                direction = 0.000000;
                positionGeo = { latitude = 33.482383, longitude = 38.654064 };
                sceneObjects = {'t:67469362'};
        };
        {
                display_name = _('NATANIA');
                beaconId = 'world_17';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'NAT';
                frequency = 112400000.000000;
                position = { -295272.531250, 21.331805, -96707.609375 };
                direction = 0.000000;
                positionGeo = { latitude = 32.333889, longitude = 34.968889 };
                sceneObjects = {'t:45942939'};
        };
        {
                display_name = _('KAHRAMANMARAS');
                beaconId = 'world_18';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'KHM';
                frequency = 113900000.000000;
                position = { 276922.218750, 503.680679, 101996.453125 };
                direction = 0.000000;
                positionGeo = { latitude = 37.540371, longitude = 36.953481 };
                sceneObjects = {'t:130383872'};
        };
        {
                display_name = _('KARIATAIN');
                beaconId = 'world_19';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'KTN';
                frequency = 117700000.000000;
                position = { -92699.039063, 737.505681, 122897.039063 };
                direction = 0.000000;
                positionGeo = { latitude = 34.213283, longitude = 37.264208 };
                sceneObjects = {'t:78544896'};
        };
        {
                display_name = _('');
                beaconId = 'airfield2_0';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IADA';
                frequency = 108700000.000000;
                position = { 220391.796875, 17.000017, -46974.203125 };
                direction = -124.198999;
                positionGeo = { latitude = 36.990667, longitude = 35.294835 };
                sceneObjects = {'t:352027538'};
                chartOffsetX = 3030.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield2_1';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IADA';
                frequency = 108700000.000000;
                position = { 218806.687500, 17.000017, -49094.089844 };
                direction = -125.000001;
                positionGeo = { latitude = 36.975663, longitude = 35.271760 };
                sceneObjects = {'t:714343684'};
        };
        {
                display_name = _('ADANA');
                beaconId = 'airfield2_2';
                type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
                callsign = 'ADN';
                frequency = 395000000.000000;
                position = { 218312.296875, 12.017589, -50018.769531 };
                direction = 0.000000;
                positionGeo = { latitude = 36.970889, longitude = 35.261611 };
                sceneObjects = {'t:351470586'};
        };
        {
                display_name = _('ADANA');
                beaconId = 'airfield2_3';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'ADA';
                frequency = 112700000.000000;
                position = { 215128.828125, 6.722708, -54714.742188 };
                direction = 0.000000;
                positionGeo = { latitude = 36.940583, longitude = 35.210389 };
                sceneObjects = {'t:122327286'};
        };
        {
                display_name = _('KALDE');
                beaconId = 'airfield6_0';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'KAD';
                frequency = 112600000.000000;
                position = { -133511.000000, 12.000012, -42518.968750 };
                direction = 0.000000;
                positionGeo = { latitude = 33.807390, longitude = 35.485991 };
                sceneObjects = {'t:73307064'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_1';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IBB';
                frequency = 110100000.000000;
                position = { -130320.203125, 12.000011, -42839.488281 };
                direction = -11.107002;
                positionGeo = { latitude = 33.836015, longitude = 35.481357 };
                sceneObjects = {'t:510821922'};
                chartOffsetX = 3722.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_2';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IKK';
                frequency = 110700000.000000;
                position = { -131222.921875, 12.000012, -40739.011719 };
                direction = -145.184003;
                positionGeo = { latitude = 33.828534, longitude = 35.504347 };
                sceneObjects = {'t:218628880'};
                chartOffsetX = 4140.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_3';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'BIL';
                frequency = 109500000.000000;
                position = { -133542.953125, 12.000012, -42243.164063 };
                direction = -1.121999;
                positionGeo = { latitude = 33.807187, longitude = 35.488977 };
                sceneObjects = {'t:218501264'};
                chartOffsetX = 3510.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_4';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IBB';
                frequency = 110100000.000000;
                position = { -133400.578125, 12.000012, -42358.453125 };
                direction = -11.106999;
                positionGeo = { latitude = 33.808433, longitude = 35.487681 };
                sceneObjects = {'t:218501253'};
                chartOffsetX = 3722.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_5';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'BIL';
                frequency = 109500000.000000;
                position = { -130440.914063, 12.000012, -42160.257813 };
                direction = -1.121999;
                positionGeo = { latitude = 33.835138, longitude = 35.488728 };
                sceneObjects = {'t:510821923'};
                chartOffsetX = 3510.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_6';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IKK';
                frequency = 110700000.000000;
                position = { -133752.968750, 12.000012, -42644.976563 };
                direction = -145.184003;
                positionGeo = { latitude = 33.805173, longitude = 35.484721 };
                sceneObjects = {'t:509281067'};
                chartOffsetX = 4140.000000;
        };
        {
                display_name = _('BEIRUT');
                beaconId = 'airfield6_7';
                type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
                callsign = 'BOD';
                frequency = 351000000.000000;
                position = { -122843.140625, 3.000003, -42540.902344 };
                direction = 0.000000;
                positionGeo = { latitude = 33.903416, longitude = 35.481814 };
                sceneObjects = {'t:224460815'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_0';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IDA';
                frequency = 109900000.000000;
                position = { -177673.234375, 612.000612, 53046.078125 };
                direction = 52.625842;
                positionGeo = { latitude = 33.434538, longitude = 36.528631 };
                sceneObjects = {'t:480608361'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IDA';
                frequency = 109900000.000000;
                position = { -180064.953125, 612.000612, 50398.371094 };
                direction = 49.999989;
                positionGeo = { latitude = 33.412415, longitude = 36.500792 };
                sceneObjects = {'t:197594063'};
                chartOffsetX = 4063.000000;
        };
        {
                display_name = _('Damascus');
                beaconId = 'airfield7_2';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'DAM';
                frequency = 116000000.000000;
                position = { -185235.656250, 610.646057, 47226.804688 };
                direction = 0.000000;
                positionGeo = { latitude = 33.365131, longitude = 36.468066 };
                sceneObjects = {'t:65082116'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_3';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'DAML';
                frequency = 111100000.000000;
                position = { -180656.640625, 612.000612, 52815.601563 };
                direction = -130.000016;
                positionGeo = { latitude = 33.407604, longitude = 36.526916 };
                sceneObjects = {'t:195693526'};
                chartOffsetX = 3926.000000;
        };
        {
                display_name = _('DAMASCUS');
                beaconId = 'airfield7_4';
                type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
                callsign = 'DAL';
                frequency = 342000000.000000;
                position = { -171986.156250, 592.527326, 60026.417969 };
                direction = 0.000000;
                positionGeo = { latitude = 33.487262, longitude = 36.602245 };
                sceneObjects = {'t:25791255'};
        };
        {
                display_name = _('ABYAD');
                beaconId = 'airfield7_5';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'ABD';
                frequency = 264000.000000;
                position = { -188453.578125, 626.805558, 43443.796875 };
                direction = 0.000000;
                positionGeo = { latitude = 33.335299, longitude = 36.428295 };
                sceneObjects = {'t:24051773'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_6';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'DAML';
                frequency = 111100000.000000;
                position = { -183014.953125, 612.000673, 50184.816406 };
                direction = -130.000001;
                positionGeo = { latitude = 33.385786, longitude = 36.499259 };
                sceneObjects = {'t:476250836'};
        };
        {
                display_name = _('HATAY');
                beaconId = 'airfield15_0';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'HTY';
                frequency = 112050000.000000;
                position = { 147753.046875, 77.282510, 39662.273438 };
                direction = -45.690008;
                positionGeo = { latitude = 36.362972, longitude = 36.290113 };
                sceneObjects = {'t:110139987'};
        };
        {
                display_name = _('');
                beaconId = 'airfield15_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IHAT';
                frequency = 108900000.000000;
                position = { 149019.375000, 78.702029, 40718.835938 };
                direction = -135.689998;
                positionGeo = { latitude = 36.374642, longitude = 36.301483 };
                sceneObjects = {'t:322309965'};
                chartOffsetX = 3421.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield15_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IHAT';
                frequency = 108900000.000000;
                position = { 146921.406250, 76.628716, 38518.476563 };
                direction = -136.812009;
                positionGeo = { latitude = 36.355193, longitude = 36.277639 };
                sceneObjects = {'t:668369721'};
        };
        {
                display_name = _('HATAY');
                beaconId = 'airfield15_3';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'HTY';
                frequency = 336000.000000;
                position = { 147771.703125, 77.308656, 39693.753906 };
                direction = -45.690008;
                positionGeo = { latitude = 36.363148, longitude = 36.290458 };
                sceneObjects = {'t:322309964'};
        };
        {
                display_name = _('');
                beaconId = 'airfield15_4';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IHTY';
                frequency = 108150000.000000;
                position = { 148442.625000, 76.732834, 40322.585938 };
                direction = 43.187996;
                positionGeo = { latitude = 36.369348, longitude = 36.297250 };
                sceneObjects = {'t:669680597'};
        };
        {
                display_name = _('');
                beaconId = 'airfield15_5';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IHTY';
                frequency = 108150000.000000;
                position = { 146243.843750, 76.015060, 38009.367188 };
                direction = 44.310007;
                positionGeo = { latitude = 36.348962, longitude = 36.272184 };
                sceneObjects = {'t:320443707'};
                chartOffsetX = 3582.000000;
        };
        {
                display_name = _('INCIRLIC');
                beaconId = 'airfield16_0';
                type = BEACON_TYPE_VORTAC;
                callsign = 'DAN';
                frequency = 108400000.000000;
                channel = 21;
                position = { 222639.437500, 73.699811, -33216.257813 };
                direction = 0.000000;
                positionGeo = { latitude = 37.015611, longitude = 35.448194 };
                sceneObjects = {'t:124814096'};
        };
        {
                display_name = _('');
                beaconId = 'airfield16_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IDAN';
                frequency = 109300000.000000;
                position = { 222315.812500, 73.638219, -33655.464844 };
                direction = -125.002995;
                positionGeo = { latitude = 37.012552, longitude = 35.443402 };
                sceneObjects = {'t:356059568'};
                chartOffsetX = 3617.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield16_2';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IDAN';
                frequency = 109300000.000000;
                position = { 220377.484375, 48.478762, -36217.171875 };
                direction = -125.000001;
                positionGeo = { latitude = 36.994247, longitude = 35.415483 };
                sceneObjects = {'t:716015361'};
                chartOffsetX = 3617.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield16_3';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'DANM';
                frequency = 111700000.000000;
                position = { 222001.125000, 63.883437, -34347.218750 };
                direction = 55.000018;
                positionGeo = { latitude = 37.009487, longitude = 35.435774 };
                sceneObjects = {'t:717392620'};
        };
        {
                display_name = _('');
                beaconId = 'airfield16_4';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'DANM';
                frequency = 111700000.000000;
                position = { 220088.968750, 46.796395, -36832.261719 };
                direction = 54.996984;
                positionGeo = { latitude = 36.991443, longitude = 35.408706 };
                sceneObjects = {'t:352452685'};
                chartOffsetX = 3639.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield21_0';
                type = BEACON_TYPE_ILS_GLIDESLOPE;
                callsign = 'IBA';
                frequency = 109100000.000000;
                position = { 43061.328125, 28.398050, 5722.375977 };
                direction = -1.444000;
                positionGeo = { latitude = 35.411243, longitude = 35.948708 };
                sceneObjects = {'t:595624788'};
        };
        {
                display_name = _('');
                beaconId = 'airfield21_1';
                type = BEACON_TYPE_ILS_LOCALIZER;
                callsign = 'IBA';
                frequency = 109100000.000000;
                position = { 40365.289063, 25.536156, 5696.175781 };
                direction = -1.444000;
                positionGeo = { latitude = 35.386960, longitude = 35.949335 };
                sceneObjects = {'t:268665151'};
                chartOffsetX = 2966.000000;
        };
        {
                display_name = _('LATAKIA');
                beaconId = 'airfield21_2';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'LTK';
                frequency = 114800000.000000;
                position = { 41444.796875, 27.958673, 5975.583496 };
                direction = 0.000000;
                positionGeo = { latitude = 35.396758, longitude = 35.952041 };
                sceneObjects = {'t:93095699'};
        };
        {
                display_name = _('LATAKIA');
                beaconId = 'airfield21_3';
                type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
                callsign = 'LTK';
                frequency = 414000000.000000;
                position = { 50737.488281, 120.053244, 5622.082031 };
                direction = -1.444000;
                positionGeo = { latitude = 35.480331, longitude = 35.944991 };
                sceneObjects = {'t:34832953'};
        };
        {
                display_name = _('PALMYRA');
                beaconId = 'airfield28_0';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'PLR';
                frequency = 363000.000000;
                position = { -55843.890625, 413.752092, 215545.687500 };
                direction = 0.000000;
                positionGeo = { latitude = 34.555739, longitude = 38.266916 };
                sceneObjects = {'t:241895405'};
        };
        {
                display_name = _('PALMYRA');
                beaconId = 'airfield28_1';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'PAL';
                frequency = 337000.000000;
                position = { -56185.023438, 389.323876, 218419.765625 };
                direction = 0.000000;
                positionGeo = { latitude = 34.552847, longitude = 38.298267 };
                sceneObjects = {'t:241928761'};
        };
        {
                display_name = _('RAMATDAVID');
                beaconId = 'airfield30_0';
                type = BEACON_TYPE_AIRPORT_HOMER;
                callsign = 'RMD';
                frequency = 368000.000000;
                position = { -259140.859375, 40.595511, -74983.906250 };
                direction = 0.000000;
                positionGeo = { latitude = 32.666274, longitude = 35.185461 };
                sceneObjects = {'t:153027415'};
        };
        {
                display_name = _('Cheka');
                beaconId = 'airfield40_0';
                type = BEACON_TYPE_VOR_DME;
                callsign = 'CAK';
                frequency = 116200000.000000;
                position = { -79459.062500, 193.868297, -20936.335938 };
                direction = 0.000000;
                positionGeo = { latitude = 34.300489, longitude = 35.699946 };
                sceneObjects = {'t:79463354'};
        };


        --MOD start

        -- Adana RWY 04
        {
                display_name = _('');
                beaconId = 'airfield2_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IADA';
                frequency = 15475000000.000000;
                channel = 3;
                position = { 220391.796875, 17.000017, -46974.203125 };
                direction = -124.198999;
                positionGeo = { latitude = 36.990667, longitude = 35.294835 };
                sceneObjects = {'t:352027538'};
                chartOffsetX = 3030.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield2_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IADA';
                frequency = 15475000000.000000;
                channel = 3;
                position = { 218806.687500, 17.000017, -49094.089844 };
                direction = -125.000001;
                positionGeo = { latitude = 36.975663, longitude = 35.271760 };
                sceneObjects = {'t:714343684'};
        };
        {
                display_name = _('');
                beaconId = 'airfield2_6';
                type = BEACON_TYPE_TACAN;
                callsign = 'IADA';
                frequency = 108700000.000000;
                channel = 24;
                position = { 218806.687500, 17.000017, -49094.089844 };
                direction = 0.000000;
                positionGeo = { latitude = 36.975663, longitude = 35.271760 };
                sceneObjects = {'t:714343684'};
        };
----------------

-- Beirut RWY 16
        {
                display_name = _('');
                beaconId = 'airfield6_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IBB';
                frequency = 15655000000.000000;
                channel = 9;
                position = { -133400.578125, 12.000012, -42358.453125 };
                direction = -11.106999;
                positionGeo = { latitude = 33.808433, longitude = 35.487681 };
                sceneObjects = {'t:218501253'};
                chartOffsetX = 3722.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_1';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IBB';
                frequency = 15655000000.000000;
                channel = 9;
                position = { -130320.203125, 12.000011, -42839.488281 };
                direction = -11.107002;
                positionGeo = { latitude = 33.836015, longitude = 35.481357 };
                sceneObjects = {'t:510821922'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_1';
                type = BEACON_TYPE_TACAN;
                callsign = 'IBB';
                frequency = 110100000.000000;
                channel = 38;
                position = { -130320.203125, 12.000011, -42839.488281 };
                direction = -11.107002;
                positionGeo = { latitude = 33.836015, longitude = 35.481357 };
                sceneObjects = {'t:510821922'};
        };

-- Beirut RWY 03        
        {
                display_name = _('');
                beaconId = 'airfield6_2';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IKK';
                frequency = 15415000000.000000;
                channel = 11;
                position = { -131222.921875, 12.000012, -40739.011719 };
                direction = -145.184003;
                positionGeo = { latitude = 33.828534, longitude = 35.504347 };
                sceneObjects = {'t:218628880'};
                chartOffsetX = 4140.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_6';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IKK';
                frequency = 15415000000.000000;
                channel = 11;
                position = { -133752.968750, 12.000012, -42644.976563 };
                direction = -145.184003;
                positionGeo = { latitude = 33.805173, longitude = 35.484721 };
                sceneObjects = {'t:509281067'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_6';
                type = BEACON_TYPE_TACAN;
                callsign = 'IKK';
                frequency = 110700000.000000;
                channel = 44;
                position = { -133752.968750, 12.000012, -42644.976563 };
                direction = -145.184003;
                positionGeo = { latitude = 33.805173, longitude = 35.484721 };
                sceneObjects = {'t:509281067'};
        };

-- Beirut RWY 17
        {
                display_name = _('');
                beaconId = 'airfield6_3';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'BIL';
                frequency = 15685000000.000000;
                channel = 10;
                position = { -133542.953125, 12.000012, -42243.164063 };
                direction = -1.121999;
                positionGeo = { latitude = 33.807187, longitude = 35.488977 };
                sceneObjects = {'t:218501264'};
                chartOffsetX = 3510.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield6_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'BIL';
                frequency = 15685000000.000000;
                channel = 10;
                position = { -130440.914063, 12.000012, -42160.257813 };
                direction = -1.121999;
                positionGeo = { latitude = 33.835138, longitude = 35.488728 };
                sceneObjects = {'t:510821923'};
        };
        {
                display_name = _('');
                beaconId = 'airfield6_5';
                type = BEACON_TYPE_TACAN;
                callsign = 'BIL';
                frequency = 109500000.000000;
                channel = 32;
                position = { -130440.914063, 12.000012, -42160.257813 };
                direction = 0.000000;
                positionGeo = { latitude = 33.835138, longitude = 35.488728 };
                sceneObjects = {'t:510821923'};
        };

----------------

-- Damascus RWY 23R
        {
                display_name = _('');
                beaconId = 'airfield7_7';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IDA';
                frequency = 15565000000.000000;
                channel = 6;
                position = { -180064.953125, 612.000612, 50398.371094 };
                direction = 49.999989;
                positionGeo = { latitude = 33.412415, longitude = 36.500792 };
                sceneObjects = {'t:197594063'};
                chartOffsetX = 4063.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield7_8';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IDA';
                frequency = 15565000000.000000;
                channel = 6;
                position = { -177673.234375, 612.000612, 53046.078125 };
                direction = 52.625842;
                positionGeo = { latitude = 33.434538, longitude = 36.528631 };
                sceneObjects = {'t:480608361'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_9';
                type = BEACON_TYPE_TACAN;
                callsign = 'IDA';
                frequency = 109900000.000000;
                channel = 36;
                position = { -177673.234375, 612.000612, 53046.078125 };
                direction = 0.000000;
                positionGeo = { latitude = 33.434538, longitude = 36.528631 };
                sceneObjects = {'t:480608361'};
        };

        -- Damascus RWY 05R

        {
                display_name = _('');
                beaconId = 'airfield7_10';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'DAML';
                frequency = 15595000000.000000;
                channel = 7;
                position = { -180656.640625, 612.000612, 52815.601563 };
                direction = -130.000016;
                positionGeo = { latitude = 33.407604, longitude = 36.526916 };
                sceneObjects = {'t:195693526'};
                chartOffsetX = 3926.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield7_11';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'DAML';
                frequency = 15595000000.000000;
                channel = 7;
                position = { -183014.953125, 612.000673, 50184.816406 };
                direction = -130.000001;
                positionGeo = { latitude = 33.385786, longitude = 36.499259 };
                sceneObjects = {'t:476250836'};
        };
        {
                display_name = _('');
                beaconId = 'airfield7_12';
                type = BEACON_TYPE_TACAN;
                callsign = 'DAML';
                frequency = 111100000.000000;
                channel = 48;
                position = { -183014.953125, 612.000673, 50184.816406 };
                direction = 0.000000;
                positionGeo = { latitude = 33.385786, longitude = 36.499259 };
                sceneObjects = {'t:476250836'};
        };
----------------

-- Hatay RWY 04
        {
                display_name = _('');
                beaconId = 'airfield15_6';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IHAT';
                frequency = 15535000000.000000;
                channel = 5;
                position = { 149019.375000, 78.702029, 40718.835938 };
                direction = -135.689998;
                positionGeo = { latitude = 36.374642, longitude = 36.301483 };
                sceneObjects = {'t:322309965'};
                chartOffsetX = 3421.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield15_7';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IHAT';
                frequency = 15535000000.000000;
                channel = 5;
                position = { 146921.406250, 76.628716, 38518.476563 };
                direction = -136.812009;
                positionGeo = { latitude = 36.355193, longitude = 36.277639 };
                sceneObjects = {'t:668369721'};
        };
        {
                display_name = _('');
                beaconId = 'airfield15_8';
                type = BEACON_TYPE_TACAN;
                callsign = 'IHAT';
                frequency = 108900000.000000;
                channel = 26;
                position = { 146921.406250, 76.628716, 38518.476563 };
                direction = 0.000000;
                positionGeo = { latitude = 36.355193, longitude = 36.277639 };
                sceneObjects = {'t:668369721'};
        };

-- Hatay RWY 22
        {
                display_name = _('');
                beaconId = 'airfield15_9';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IHTY';
                frequency = 15505000000.000000;
                channel = 4;
                position = { 146243.843750, 76.015060, 38009.367188 };
                direction = 44.310007;
                positionGeo = { latitude = 36.348962, longitude = 36.272184 };
                sceneObjects = {'t:320443707'};
                chartOffsetX = 3582.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield15_10';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IHTY';
                frequency = 15505000000.000000;
                channel = 4;
                position = { 148442.625000, 76.732834, 40322.585938 };
                direction = 43.187996;
                positionGeo = { latitude = 36.369348, longitude = 36.297250 };
                sceneObjects = {'t:669680597'};
        };
        {
                display_name = _('');
                beaconId = 'airfield15_11';
                type = BEACON_TYPE_TACAN;
                callsign = 'IHTY';
                frequency = 108150000.000000;
                channel = 18;
                position = { 148442.625000, 76.732834, 40322.585938 };
                direction = 0.000000;
                positionGeo = { latitude = 36.369348, longitude = 36.297250 };
                sceneObjects = {'t:669680597'};
        };
----------------

--Incirlik RWY 05        
        {
                display_name = _('');
                beaconId = 'airfield16_5';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IDAN';
                frequency = 15445000000.000000;
                channel = 2;
                position = { 222315.812500, 73.638219, -33655.464844 };
                direction = -125.002995;
                positionGeo = { latitude = 37.012552, longitude = 35.443402 };
                sceneObjects = {'t:356059568'};
                chartOffsetX = 3617.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield16_6';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IDAN';
                frequency = 15445000000.000000;
                channel = 2;
                position = { 220377.484375, 48.478762, -36217.171875 };
                direction = -125.000001;
                positionGeo = { latitude = 36.994247, longitude = 35.415483 };
                sceneObjects = {'t:716015361'};
        };
        {
                display_name = _('');
                beaconId = 'airfield16_7';
                type = BEACON_TYPE_TACAN;
                callsign = 'IDAN';
                frequency = 109300000.000000;
                channel = 30;
                position = { 220377.484375, 48.478762, -36217.171875 };
                direction = 0.000000;
                positionGeo = { latitude = 36.994247, longitude = 35.415483 };
                sceneObjects = {'t:716015361'};
        };
-- Incirlik RWY 23

        {
                display_name = _('');
                beaconId = 'airfield16_8';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'DANM';
                frequency = 15415000000.000000;
                channel = 1;
                position = { 220088.968750, 46.796395, -36832.261719 };
                direction = 54.996984;
                positionGeo = { latitude = 36.991443, longitude = 35.408706 };
                sceneObjects = {'t:352452685'};
                chartOffsetX = 3639.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield16_9';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'DANM';
                frequency = 15415000000.000000;
                channel = 1;
                position = { 222001.125000, 63.883437, -34347.218750 };
                direction = 55.000018;
                positionGeo = { latitude = 37.009487, longitude = 35.435774 };
                sceneObjects = {'t:717392620'};
        };
        {
                display_name = _('');
                beaconId = 'airfield16_10';
                type = BEACON_TYPE_TACAN;
                callsign = 'DANM';
                frequency = 111700000.000000;
                channel = 54;
                position = { 222001.125000, 63.883437, -34347.218750 };
                direction = 0.000000;
                positionGeo = { latitude = 37.009487, longitude = 35.435774 };
                sceneObjects = {'t:717392620'};
        };

----------------

-- Khmeimim/Bassel Al-Assad RWY 17R
        {
                display_name = _('');
                beaconId = 'airfield21_4';
                type = BEACON_TYPE_ICLS_LOCALIZER;
                callsign = 'IBA';
                frequency = 15625000000.000000;
                channel = 8;
                position = { 40365.289063, 25.536156, 5696.175781 };
                direction = -1.444000;
                positionGeo = { latitude = 35.386960, longitude = 35.949335 };
                sceneObjects = {'t:268665151'};
                chartOffsetX = 2966.000000;
        };
        {
                display_name = _('');
                beaconId = 'airfield21_5';
                type = BEACON_TYPE_ICLS_GLIDESLOPE;
                callsign = 'IBA';
                frequency = 15625000000.000000;
                channel = 8;
                position = { 43061.328125, 28.398050, 5722.375977 };
                direction = -1.444000;
                positionGeo = { latitude = 35.411243, longitude = 35.948708 };
                sceneObjects = {'t:595624788'};
        };
        {
                display_name = _('');
                beaconId = 'airfield21_6';
                type = BEACON_TYPE_TACAN;
                callsign = 'IBA';
                frequency = 109100000.000000;
                channel = 28;
                position = { 43061.328125, 28.398050, 5722.375977 };
                direction = 0.000000;
                positionGeo = { latitude = 35.411243, longitude = 35.948708 };
                sceneObjects = {'t:595624788'};
        };

----------------


}
    branches: [ "main" ]
  pull_request: 
#tell the linker the rpath so that we don't have to muck with LD_LIBRARY_PATH, etc
dramsim_test: dramsim_test.cpp
        $(CXX) -g -o dramsim_test dramsim_test.cpp -I../ -L../ -ldramsim -Wl,-rpath=../

clean: 
        rm dramsim_test
    branches: [ "main" ]
  schedule: /*********************************************************************************
*  Copyright (c) 2010-2011, 
*                             
*                             
*                             University of Maryland 
*                             dramninjas [at] gmail [dot] com
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/




#include <stdio.h>
#include "dramsim_test.h"

using namespace DRAMSim;

/* callback functors */
void some_object::read_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
        printf("[Callback] read complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

void some_object::write_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
        printf("[Callback] write complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

/* FIXME: this may be broken, currently */
void power_callback(double a, double b, double c, double d)
{
//        printf("power callback: %0.3f, %0.3f, %0.3f, %0.3f\n",a,b,c,d);
}

int some_object::add_one_and_run(MultiChannelMemorySystem *mem, uint64_t addr)
{

        /* create a transaction and add it */
        bool isWrite = false; 
        mem->addTransaction(isWrite, addr);

        // send a read to channel 1 on the same cycle 
        addr = 1LL<<33 | addr; 
        mem->addTransaction(isWrite, addr);

        for (int i=0; i<5; i++)
        {
                mem->update();
        }

        /* add another some time in the future */

        // send a write to channel 0 
        addr = 0x900012; 
        isWrite = true; 
        mem->addTransaction(isWrite, addr);


        /* do a bunch of updates (i.e. clocks) -- at some point the callback will fire */
        for (int i=0; i<45; i++)
        {
                mem->update();
        }

        /* get a nice summary of this epoch */
        mem->printStats(true);

        return 0;
}

int main()
{
        some_object obj;
        TransactionCompleteCB *read_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::read_complete);
        TransactionCompleteCB *write_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::write_complete);

        /* pick a DRAM part to simulate */
        MultiChannelMemorySystem *mem = getMemorySystemInstance("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "system.ini", "..", "example_app", 16384); 


        mem->RegisterCallbacks(read_cb, write_cb, power_callback);
        MultiChannelMemorySystem *mem2 = getMemorySystemInstance("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "system.ini", "..", "example_app", 16384); 

        mem2->RegisterCallbacks(read_cb, write_cb, power_callback);

        printf("dramsim_test main()\n");
        printf("-----MEM1------\n");
        obj.add_one_and_run(mem, 0x100001UL);
        obj.add_one_and_run(mem, 0x200002UL);

        printf("-----MEM2------\n");
        obj.add_one_and_run(mem2, 0x300002UL);
        return 0; 
}

    - cron: /*********************************************************************************
*  Copyright (c) 2010-2011, 
*                             
*                             
*                             University of Maryland 
*                             dramninjas [at] gmail [dot] com
*  All rights reserved.
*  
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  
*     * Redistributions of source code must retain the above copyright notice,
*        this list of conditions and the following disclaimer.
*  
*     * Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
*  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************/




#include <string>
#include <stdint.h>
#include <DRAMSim.h>

class some_object
{
        public: 
                void read_complete(unsigned, uint64_t, uint64_t);
                void write_complete(unsigned, uint64_t, uint64_t);
                int add_one_and_run(DRAMSim::MultiChannelMemorySystem *mem, uint64_t addr);
};

env: ; blah blah
NUM_BANKS=8
NUM_ROWS=16384 
NUM_COLS=1024
DEVICE_WIDTH=8

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=3.0 ;

CL=4 ;
AL=0 ;
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=4 ;
tRAS=14; 
tRCD=4 ;
tRRD=3 ;
tRC=18 ;
tRP=4  ;
tCCD=2 ;
tRTP=3 ;
tWTR=3 ;
tWR=5  ;
tRTRS=1;
tRFC=43;
tFAW=13;
tCKE=3 ;
tXP=2  ;

tCMD=1 ;

IDD0=85
IDD1=100
IDD2P=7;
IDD2Q=40;
IDD2N=40;
IDD3Pf=30;
IDD3Ps=10;
IDD3N=55;
IDD4W=135;
IDD4R=135;
IDD5=215;
IDD6=7;
IDD6L=5;
IDD7=280;
Vdd=1.8 ; TODO: double check this
  # Path to the CMake build directory.
  build: ; This is for a micron DDR2, -3E part as per the verilog models
NUM_BANKS=4
NUM_ROWS=16384 
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=3.0 ;*

CL=4 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=4 ;*
tRAS=14;* 
tRCD=4 ;*
tRRD=3 ;*
tRC=18 ;*
tRP=4  ;*
tCCD=2 ;*
tRTP=3 ;*
tWTR=3 ;*
tWR=5  ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=35;*
tFAW=13;*
tCKE=3 ;*
tXP=2  ;*

tCMD=1 ;*

IDD0=90;
IDD1=105;
IDD2P=7;
IDD2Q=45;
IDD2N=50;
IDD3Pf=35;
IDD3Ps=12;
IDD3N=65;
IDD4W=170;
IDD4R=180;
IDD5=180;
IDD6=7;
IDD6L=3;
IDD7=240;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank
Vdd=1.8 ; TODO: double check this

permissions: ; This is for a micron DDR2, -3E part as per the verilog models
NUM_BANKS=8
NUM_ROWS=16384 
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=2.5 ;*

CL=5 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=4 ;*
tRAS=18;* 
tRCD=5 ;*
tRRD=3 ;*
tRC=23 ;*
tRP=5 ;*
tCCD=2 ;*
tRTP=3 ;*
tWTR=3 ;*
tWR=6 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=51;*
tFAW=14;*
tCKE=3 ;*
tXP=2  ;*

tCMD=1 ;*

IDD0=90;
IDD1=110;
IDD2P=7;
IDD2Q=45;
IDD2N=50;
IDD3Pf=35;
IDD3Ps=12;
IDD3N=60;
IDD4W=145;
IDD4R=145;
IDD5=235;
IDD6=7;
IDD6L=3;
IDD7=335;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank
Vdd=1.8 ; TODO: double check this
  contents: NUM_BANKS=8
NUM_ROWS=16384
NUM_COLS=1024
DEVICE_WIDTH=16

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=74;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=110;
IDD1=130;
IDD2P=12;
IDD2Q=60;
IDD2N=65;
IDD3Pf=40;
IDD3Ps=40;
IDD3N=62;
IDD4W=220;
IDD4R=200;
IDD5=240;
IDD6=6;
IDD6L=9;
IDD7=490;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this

jobs: ; This parts is taken from the micron MT41J256M4 datasheet ( http://micron.com/document_download/?documentId=425 )

NUM_BANKS=8
NUM_ROWS=16384
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
REFRESH_PERIOD=7800
tCK=1.25

CL=11
AL=0
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8
tRAS=28
tRCD=11
tRRD=5 ; actually 6ns=4.8CK, but rounded up
tRC=39
tRP=11
tCCD=4
tRTP=6 ; 7.5ns
tWTR=6 ; 7.5ns
tWR=12 ; 15ns
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=88
tFAW=24 ; This part has 1KB (2k columns x 4) = 30ns
tCKE=4 ; 5ns
tXP=5 ; 6ns = 4.8CK rounded up

tCMD=1 ;*

; x4 width; DDR3-1660; page 44
IDD0=95
IDD1=115
IDD2P=45 ; assuming 'fast mode'
IDD2Q=67
IDD2N=70
IDD3Pf=45 ; unused -- also DDR3 doesn't have f,s versions 
IDD3Ps=45 ; also unused
IDD3N=67  
IDD4W=250
IDD4R=250
IDD5=260
IDD6=6 ; this is unused
IDD6L=6 ; this is unused
IDD7=400 ; this is unused

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this
  analyze: NUM_BANKS=8
NUM_ROWS=16384
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=74;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=85;
IDD1=105;
IDD2P=12;
IDD2Q=60;
IDD2N=65;
IDD3Pf=40;
IDD3Ps=40;
IDD3N=62;
IDD4W=220;
IDD4R=200;
IDD5=240;
IDD6=6;
IDD6L=9;
IDD7=315;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this
    permissions: NUM_BANKS=8
NUM_ROWS=32768
NUM_COLS=1024
DEVICE_WIDTH=8

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=107;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=130;
IDD1=155;
IDD2P=10;
IDD2Q=70;
IDD2N=70;
IDD3Pf=60;
IDD3Ps=60;
IDD3N=90;
IDD4W=300;
IDD4R=255;
IDD5=305;
IDD6=9;
IDD6L=12;
IDD7=460;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this
      contents: NUM_BANKS=8
NUM_ROWS=32768
NUM_COLS=1024
DEVICE_WIDTH=8

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=2.5 ;*

CL=5 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=15;* 
tRCD=5 ;*
tRRD=4 ;*
tRC=20 ;*
tRP=5  ;*
tCCD=4 ;*
tRTP=4 ;*
tWTR=4 ;*
tWR=6 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=64;*
tFAW=16;*
tCKE=3 ;*
tXP=3 ;*

tCMD=1 ;*

IDD0=100;
IDD1=115;
IDD2P=10;
IDD2Q=50;
IDD2N=50;
IDD3Pf=45;
IDD3Ps=45;
IDD3N=65;
IDD4W=230;
IDD4R=195;
IDD5=275;
IDD6=9;
IDD6L=12;
IDD7=400;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this # for actions/checkout to fetch code
      security-events: NUM_BANKS=8
NUM_ROWS=32768
NUM_COLS=2048
DEVICE_WIDTH=4

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=107;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=100;
IDD1=130;
IDD2P=10;
IDD2Q=70;
IDD2N=70;
IDD3Pf=60;
IDD3Ps=60;
IDD3N=90;
IDD4W=255;
IDD4R=230;
IDD5=305;
IDD6=9;
IDD6L=12;
IDD7=415;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this # for github/codeql-action/upload-sarif to upload SARIF results
      actions: NUM_BANKS=8
NUM_ROWS=8192
NUM_COLS=1024
DEVICE_WIDTH=16

;in nanoseconds
;#define REFRESH_PERIOD 7800
REFRESH_PERIOD=7800
tCK=1.5 ;*

CL=10 ;*
AL=0 ;*
;AL=3; needs to be tRCD-1 or 0
;RL=(CL+AL)
;WL=(RL-1)
BL=8 ;*
tRAS=24;* 
tRCD=10 ;*
tRRD=4 ;*
tRC=34 ;*
tRP=10  ;*
tCCD=4 ;*
tRTP=5 ;*
tWTR=5 ;*
tWR=10 ;*
tRTRS=1; -- RANK PARAMETER, TODO 
tRFC=74;*
tFAW=20;*
tCKE=4 ;*
tXP=4 ;*

tCMD=1 ;*

IDD0=110;
IDD1=150;
IDD2P=12;
IDD2Q=60;
IDD2N=65;
IDD3Pf=40;
IDD3Ps=40;
IDD3N=60;
IDD4W=355;
IDD4R=290;
IDD5=240;
IDD6=6;
IDD6L=9;
IDD7=420;

;same bank
;READ_TO_PRE_DELAY=(AL+BL/2+max(tRTP,2)-2)
;WRITE_TO_PRE_DELAY=(WL+BL/2+tWR)
;READ_TO_WRITE_DELAY=(RL+BL/2+tRTRS-WL)
;READ_AUTOPRE_DELAY=(AL+tRTP+tRP)
;WRITE_AUTOPRE_DELAY=(WL+BL/2+tWR+tRP)
;WRITE_TO_READ_DELAY_B=(WL+BL/2+tWTR);interbank
;WRITE_TO_READ_DELAY_R=(WL+BL/2+tRTRS-RL);interrank

Vdd=1.5 ; TODO: double check this # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
;
;The copyright holder is BSC-CNS, and the authorship correspond to Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. The complete explanation of the derivation of the data can be found in the following study: Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. 2017. Enabling a reliable STT-MRAM main memory simulation. In Proceedings of the International Symposium on Memory Systems (MEMSYS '17). Washington DC, USA, 283-292. DOI: https://doi.org/10.1145/3132402.3132416

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

;2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”

; This configuration lists detailed timing parameters for STT-MRAM main memory, specifying a 1.2x deviation from respective DRAM timing parameters. Please note, the current parameters (IDD0, IDD1.. etc) DOES NOT correspond to STT-MRAM and should not be used for current/energy estimations.    

NUM_BANKS=8;    
NUM_ROWS=32768; 
NUM_COLS=1024;  

DEVICE_WIDTH=8; 

REFRESH_PERIOD=7800;
tCK=1.25; 

CL=11; 
AL=0; 

BL=4;            
tRAS=20;         
tRCD=14;        
tRRD=6;         
tRC=34;         
tRP=14;        
tCCD=4;
tRTP=6;
tWTR=6;  
tWR=12; 
tRTRS=1;         
tRFC=1;      
tFAW=29;      
tCKE=4;        
tXP=5;          
tCMD=1;

; The following current parameters DOES NOT correspond to STT-MRAM, and should not be used used for current/energy estimations.    
IDD0=1305;      
IDD1=1395;     
IDD2P=846;      
IDD2Q=1030;     
IDD2N=1050;     
IDD3Pf=60;     
IDD3Ps=60;      
IDD3N=1310;    
IDD4W=1765;     
IDD4R=230;      
IDD5=1940;     
IDD6=246;       
IDD6L=246;     
IDD7=2160;      

Vdd=1.5; 
    runs-on: ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
;
;The copyright holder is BSC-CNS, and the authorship correspond to Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. The complete explanation of the derivation of the data can be found in the following study: Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. 2017. Enabling a reliable STT-MRAM main memory simulation. In Proceedings of the International Symposium on Memory Systems (MEMSYS '17). Washington DC, USA, 283-292. DOI: https://doi.org/10.1145/3132402.3132416

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

;2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”

; This configuration lists detailed timing parameters for STT-MRAM main memory, specifying a 1.5x deviation from respective DRAM timing parameters. Please note, the current parameters (IDD0, IDD1.. etc) DOES NOT correspond to STT-MRAM and should not be used for current/energy estimations.    

NUM_BANKS=8;    
NUM_ROWS=32768; 
NUM_COLS=1024;  

DEVICE_WIDTH=8; 

REFRESH_PERIOD=7800; 
tCK=1.25;

CL=11; 
AL=0; 

BL=4;           
tRAS=23;        
tRCD=17;        
tRRD=8;         
tRC=40;         
tRP=17;        
tCCD=4;         
tRTP=6;        
tWTR=6;         
tWR=12;         
tRTRS=1;         
tRFC=1;         
tFAW=36;        
tCKE=4;         
tXP=5;         
tCMD=1; 

; The following current parameters DOES NOT correspond to STT-MRAM, and should not be used used for current/energy estimations.    
IDD0=1305;      
IDD1=1395;      
IDD2P=846;      
IDD2Q=1030;     
IDD2N=1050;      
IDD3Pf=60;      
IDD3Ps=60;      
IDD3N=1310;     
IDD4W=1765;     
IDD4R=230;      
IDD5=1940;      
IDD6=246;       
IDD6L=246;      
IDD7=2160;      

Vdd=1.5; 


    steps: ;  (C) Copyright 2006-2018 Barcelona Supercomputing Center (BSC) 
;
;The copyright holder is BSC-CNS, and the authorship correspond to Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. The complete explanation of the derivation of the data can be found in the following study: Kazi Asifuzzaman, Rommel Sanchez Verdejo, and Petar Radojkovic. 2017. Enabling a reliable STT-MRAM main memory simulation. In Proceedings of the International Symposium on Memory Systems (MEMSYS '17). Washington DC, USA, 283-292. DOI: https://doi.org/10.1145/3132402.3132416

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

;2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.”

; This configuration lists detailed timing parameters for STT-MRAM main memory, specifying a 2x deviation from respective DRAM timing parameters. Please note, the current parameters (IDD0, IDD1.. etc) DOES NOT correspond to STT-MRAM and should not be used for current/energy estimations.    

NUM_BANKS=8;    
NUM_ROWS=32768; 
NUM_COLS=1024;  

DEVICE_WIDTH=8; 

REFRESH_PERIOD=7800; 
tCK=1.25; 

CL=11; 
AL=0; 

BL=4;           
tRAS=28;        
tRCD=22;        
tRRD=10;        
tRC=50;         
tRP=22;         
tCCD=4;         
tRTP=6;         
tWTR=6;         
tWR=12;         
tRTRS=1;         
tRFC=1;         
tFAW=48;        
tCKE=4;         
tXP=5;          
tCMD=1; 

; The following current parameters DOES NOT correspond to STT-MRAM, and should not be used used for current/energy estimations.
IDD0=1305;      
IDD1=1395;      
IDD2P=846;      
IDD2Q=1030;     
IDD2N=1050;     
IDD3Pf=60;      
IDD3Ps=60;      
IDD3N=1310;     
IDD4W=1765;     
IDD4R=230;     
IDD5=1940;     
IDD6=246;      
IDD6L=246;      
IDD7=2160;      

Vdd=1.5; 
      - name: import torch
import torch.nn as nn
import numpy as np
from pytorch3d.structures import Meshes
from core.BaseModel import BaseReconModel
from pytorch3d.renderer import (
    look_at_view_transform,
    FoVPerspectiveCameras,
    PointLights,
    RasterizationSettings,
    MeshRenderer,
    MeshRasterizer,
    SoftPhongShader,
    TexturesVertex,
    blending
)


class BFM09ReconModel(BaseReconModel):
    def __init__(self, model_dict, **kargs):
        super(BFM09ReconModel, self).__init__(**kargs)

        self.skinmask = torch.tensor(
            model_dict['skinmask'], requires_grad=False, device=self.device)

        self.kp_inds = torch.tensor(
            model_dict['keypoints']-1).squeeze().long().to(self.device)

        self.meanshape = torch.tensor(model_dict['meanshape'],
                                      dtype=torch.float32, requires_grad=False,
                                      device=self.device)

        self.idBase = torch.tensor(model_dict['idBase'],
                                   dtype=torch.float32, requires_grad=False,
                                   device=self.device)

        self.expBase = torch.tensor(model_dict['exBase'],
                                    dtype=torch.float32, requires_grad=False,
                                    device=self.device)

        self.meantex = torch.tensor(model_dict['meantex'],
                                    dtype=torch.float32, requires_grad=False,
                                    device=self.device)

        self.texBase = torch.tensor(model_dict['texBase'],
                                    dtype=torch.float32, requires_grad=False,
                                    device=self.device)

        self.tri = torch.tensor(model_dict['tri']-1,
                                dtype=torch.int64, requires_grad=False,
                                device=self.device)

        self.point_buf = torch.tensor(model_dict['point_buf']-1,
                                      dtype=torch.int64, requires_grad=False,
                                      device=self.device)

    def get_lms(self, vs):
        lms = vs[:, self.kp_inds, :]
        return lms

    def split_coeffs(self, coeffs):
        id_coeff = coeffs[:, :80]  # identity(shape) coeff of dim 80
        exp_coeff = coeffs[:, 80:144]  # expression coeff of dim 64
        tex_coeff = coeffs[:, 144:224]  # texture(albedo) coeff of dim 80
        # ruler angles(x,y,z) for rotation of dim 3
        angles = coeffs[:, 224:227]
        # lighting coeff for 3 channel SH function of dim 27
        gamma = coeffs[:, 227:254]
        translation = coeffs[:, 254:]  # translation coeff of dim 3

        return id_coeff, exp_coeff, tex_coeff, angles, gamma, translation

    def merge_coeffs(self, id_coeff, exp_coeff, tex_coeff, angles, gamma, translation):
        coeffs = torch.cat([id_coeff, exp_coeff, tex_coeff,
                            angles, gamma, translation], dim=1)
        return coeffs

    def forward(self, coeffs, render=True):
        batch_num = coeffs.shape[0]

        id_coeff, exp_coeff, tex_coeff, angles, gamma, translation = self.split_coeffs(
            coeffs)

        vs = self.get_vs(id_coeff, exp_coeff)

        rotation = self.compute_rotation_matrix(angles)

        vs_t = self.rigid_transform(
            vs, rotation, translation)

        lms_t = self.get_lms(vs_t)
        lms_proj = self.project_vs(lms_t)
        lms_proj = torch.stack(
            [lms_proj[:, :, 0], self.img_size-lms_proj[:, :, 1]], dim=2)
        if render:
            face_texture = self.get_color(tex_coeff)
            face_norm = self.compute_norm(vs, self.tri, self.point_buf)
            face_norm_r = face_norm.bmm(rotation)
            face_color = self.add_illumination(
                face_texture, face_norm_r, gamma)
            face_color_tv = TexturesVertex(face_color)

            mesh = Meshes(vs_t, self.tri.repeat(
                batch_num, 1, 1), face_color_tv)
            rendered_img = self.renderer(mesh)
            rendered_img = torch.clamp(rendered_img, 0, 255)

            return {'rendered_img': rendered_img,
                    'lms_proj': lms_proj,
                    'face_texture': face_texture,
                    'vs': vs_t,
                    'tri': self.tri,
                    'color': face_color}
        else:
            return {'lms_proj': lms_proj}

    def get_vs(self, id_coeff, exp_coeff):
        n_b = id_coeff.size(0)

        face_shape = torch.einsum('ij,aj->ai', self.idBase, id_coeff) + \
            torch.einsum('ij,aj->ai', self.expBase, exp_coeff) + self.meanshape

        face_shape = face_shape.view(n_b, -1, 3)
        face_shape = face_shape - \
            self.meanshape.view(1, -1, 3).mean(dim=1, keepdim=True)

        return face_shape

    def get_color(self, tex_coeff):
        n_b = tex_coeff.size(0)
        face_texture = torch.einsum(
            'ij,aj->ai', self.texBase, tex_coeff) + self.meantex

        face_texture = face_texture.view(n_b, -1, 3)
        return face_texture

    def get_skinmask(self):
        return self.skinmask

    def init_coeff_dims(self):
        self.id_dims = 80
        self.tex_dims = 80
        self.exp_dims = 64
        uses: import torch
import torch.nn as nn
import numpy as np
from pytorch3d.structures import Meshes
from pytorch3d.renderer import (
    look_at_view_transform,
    FoVPerspectiveCameras,
    PointLights,
    RasterizationSettings,
    MeshRenderer,
    MeshRasterizer,
    SoftPhongShader,
    TexturesVertex,
    blending
)


class BaseReconModel(nn.Module):
    def __init__(self, batch_size=1,
                 focal=1015, img_size=224, device='cuda:0'):
        super(BaseReconModel, self).__init__()

        self.focal = focal
        self.batch_size = batch_size
        self.img_size = img_size

        self.device = torch.device(device)
        self.renderer = self._get_renderer(self.device)

        self.p_mat = self._get_p_mat(device)
        self.reverse_z = self._get_reverse_z(device)
        self.camera_pos = self._get_camera_pose(device)

        self.rot_tensor = None
        self.exp_tensor = None
        self.id_tensor = None
        self.tex_tensor = None
        self.trans_tensor = None
        self.gamma_tensor = None

        self.init_coeff_dims()

        self.init_coeff_tensors()

    def _get_camera_pose(self, device):
        camera_pos = torch.tensor(
            [0.0, 0.0, 10.0], device=device).reshape(1, 1, 3)
        return camera_pos

    def _get_p_mat(self, device):
        half_image_width = self.img_size // 2
        p_matrix = np.array([self.focal, 0.0, half_image_width,
                             0.0, self.focal, half_image_width,
                             0.0, 0.0, 1.0], dtype=np.float32).reshape(1, 3, 3)
        return torch.tensor(p_matrix, device=device)

    def _get_reverse_z(self, device):
        reverse_z = np.reshape(
            np.array([1.0, 0, 0, 0, 1, 0, 0, 0, -1.0], dtype=np.float32), [1, 3, 3])
        return torch.tensor(reverse_z, device=device)

    def _get_renderer(self, device):
        R, T = look_at_view_transform(10, 0, 0)  # camera's position
        cameras = FoVPerspectiveCameras(device=device, R=R, T=T, znear=0.01,
                                        zfar=50,
                                        fov=2*np.arctan(self.img_size//2/self.focal)*180./np.pi)

        lights = PointLights(device=device, location=[[0.0, 0.0, 1e5]],
                             ambient_color=[[1, 1, 1]],
                             specular_color=[[0., 0., 0.]], diffuse_color=[[0., 0., 0.]])

        raster_settings = RasterizationSettings(
            image_size=self.img_size,
            blur_radius=0.0,
            faces_per_pixel=1,
        )
        blend_params = blending.BlendParams(background_color=[0, 0, 0])

        renderer = MeshRenderer(
            rasterizer=MeshRasterizer(
                cameras=cameras,
                raster_settings=raster_settings
            ),
            shader=SoftPhongShader(
                device=device,
                cameras=cameras,
                lights=lights,
                blend_params=blend_params
            )
        )
        return renderer

    def compute_norm(self, vs, tri, point_buf):

        face_id = tri
        point_id = point_buf
        v1 = vs[:, face_id[:, 0], :]
        v2 = vs[:, face_id[:, 1], :]
        v3 = vs[:, face_id[:, 2], :]
        e1 = v1 - v2
        e2 = v2 - v3
        face_norm = e1.cross(e2)
        empty = torch.zeros((face_norm.size(0), 1, 3),
                            dtype=face_norm.dtype, device=face_norm.device)
        face_norm = torch.cat((face_norm, empty), 1)
        v_norm = face_norm[:, point_id, :].sum(2)
        v_norm = v_norm / v_norm.norm(dim=2).unsqueeze(2)

        return v_norm

    def project_vs(self, vs):
        batchsize = vs.shape[0]

        vs = torch.matmul(vs, self.reverse_z.repeat(
            (batchsize, 1, 1))) + self.camera_pos
        aug_projection = torch.matmul(
            vs, self.p_mat.repeat((batchsize, 1, 1)).permute((0, 2, 1)))

        face_projection = aug_projection[:, :, :2] / \
            torch.reshape(aug_projection[:, :, 2], [batchsize, -1, 1])
        return face_projection

    def compute_rotation_matrix(self, angles):
        n_b = angles.size(0)
        sinx = torch.sin(angles[:, 0])
        siny = torch.sin(angles[:, 1])
        sinz = torch.sin(angles[:, 2])
        cosx = torch.cos(angles[:, 0])
        cosy = torch.cos(angles[:, 1])
        cosz = torch.cos(angles[:, 2])

        rotXYZ = torch.eye(3).view(1, 3, 3).repeat(
            n_b * 3, 1, 1).view(3, n_b, 3, 3).to(angles.device)

        rotXYZ[0, :, 1, 1] = cosx
        rotXYZ[0, :, 1, 2] = -sinx
        rotXYZ[0, :, 2, 1] = sinx
        rotXYZ[0, :, 2, 2] = cosx
        rotXYZ[1, :, 0, 0] = cosy
        rotXYZ[1, :, 0, 2] = siny
        rotXYZ[1, :, 2, 0] = -siny
        rotXYZ[1, :, 2, 2] = cosy
        rotXYZ[2, :, 0, 0] = cosz
        rotXYZ[2, :, 0, 1] = -sinz
        rotXYZ[2, :, 1, 0] = sinz
        rotXYZ[2, :, 1, 1] = cosz

        rotation = rotXYZ[2].bmm(rotXYZ[1]).bmm(rotXYZ[0])

        return rotation.permute(0, 2, 1)

    def add_illumination(self, face_texture, norm, gamma):

        n_b, num_vertex, _ = face_texture.size()
        n_v_full = n_b * num_vertex
        gamma = gamma.view(-1, 3, 9).clone()
        gamma[:, :, 0] += 0.8

        gamma = gamma.permute(0, 2, 1)

        a0 = np.pi
        a1 = 2 * np.pi / np.sqrt(3.0)
        a2 = 2 * np.pi / np.sqrt(8.0)
        c0 = 1 / np.sqrt(4 * np.pi)
        c1 = np.sqrt(3.0) / np.sqrt(4 * np.pi)
        c2 = 3 * np.sqrt(5.0) / np.sqrt(12 * np.pi)
        d0 = 0.5 / np.sqrt(3.0)

        Y0 = torch.ones(n_v_full).to(gamma.device).float() * a0 * c0
        norm = norm.view(-1, 3)
        nx, ny, nz = norm[:, 0], norm[:, 1], norm[:, 2]
        arrH = []

        arrH.append(Y0)
        arrH.append(-a1 * c1 * ny)
        arrH.append(a1 * c1 * nz)
        arrH.append(-a1 * c1 * nx)
        arrH.append(a2 * c2 * nx * ny)
        arrH.append(-a2 * c2 * ny * nz)
        arrH.append(a2 * c2 * d0 * (3 * nz.pow(2) - 1))
        arrH.append(-a2 * c2 * nx * nz)
        arrH.append(a2 * c2 * 0.5 * (nx.pow(2) - ny.pow(2)))

        H = torch.stack(arrH, 1)
        Y = H.view(n_b, num_vertex, 9)
        lighting = Y.bmm(gamma)

        face_color = face_texture * lighting
        return face_color

    def rigid_transform(self, vs, rot, trans):

        vs_r = torch.matmul(vs, rot)
        vs_t = vs_r + trans.view(-1, 1, 3)

        return vs_t

    def get_lms(self, vs):
        raise NotImplementedError()

    def forward(self, coeffs, render=True):
        raise NotImplementedError()

    def get_vs(self, id_coeff, exp_coeff):
        raise NotImplementedError()

    def get_color(self, tex_coeff):
        raise NotImplementedError()

    def get_rot_tensor(self):
        return self.rot_tensor

    def get_trans_tensor(self):
        return self.trans_tensor

    def get_exp_tensor(self):
        return self.exp_tensor

    def get_tex_tensor(self):
        return self.tex_tensor

    def get_id_tensor(self):
        return self.id_tensor

    def get_gamma_tensor(self):
        return self.gamma_tensor

    def init_coeff_dims(self):
        raise NotImplementedError()

    def get_packed_tensors(self):
        return self.merge_coeffs(self.id_tensor.repeat(self.batch_size, 1),
                                 self.exp_tensor,
                                 self.tex_tensor.repeat(self.batch_size, 1),
                                 self.rot_tensor, self.gamma_tensor,
                                 self.trans_tensor)

    def init_coeff_tensors(self, id_coeff=None, tex_coeff=None):
        if id_coeff is None:
            self.id_tensor = torch.zeros(
                (1, self.id_dims), dtype=torch.float32,
                requires_grad=True, device=self.device)
        else:
            assert id_coeff.shape == (1, self.id_dims)
            self.id_tensor = torch.tensor(
                id_coeff, dtype=torch.float32,
                requires_grad=True, device=self.device)

        if tex_coeff is None:
            self.tex_tensor = torch.zeros(
                (1, self.tex_dims), dtype=torch.float32,
                requires_grad=True, device=self.device)
        else:
            assert tex_coeff.shape == (1, self.tex_dims)
            self.tex_tensor = torch.tensor(
                tex_coeff, dtype=torch.float32,
                requires_grad=True, device=self.device)

        self.exp_tensor = torch.zeros(
            (self.batch_size, self.exp_dims), dtype=torch.float32,
            requires_grad=True, device=self.device)
        self.gamma_tensor = torch.zeros(
            (self.batch_size, 27), dtype=torch.float32,
            requires_grad=True, device=self.device)
        self.trans_tensor = torch.zeros(
            (self.batch_size, 3), dtype=torch.float32,
            requires_grad=True, device=self.device)
        self.rot_tensor = torch.zeros(
            (self.batch_size, 3), dtype=torch.float32,
            requires_grad=True, device=self.device)

      - name: from core.BFM09Model import BFM09ReconModel
from scipy.io import loadmat


def get_recon_model(model='bfm09', **kargs):
    if model == 'bfm09':
        model_path = 'BFM/BFM09_model_info.mat'
        model_dict = loadmat(model_path)
        recon_model = BFM09ReconModel(model_dict, **kargs)
        return recon_model
    else:
        raise NotImplementedError()
        run: 
import numpy as np
from torch.utils import data
import torch
import os
import pickle
import cv2


class FittingDataset(data.Dataset):

    def __init__(self, img_folder, pkl_path, worker_num=1, worker_ind=0):
        super(FittingDataset, self).__init__()
        with open(pkl_path, 'rb') as f:
            lm_dict = pickle.load(f)
        self.lm_dict = lm_dict
        keys = list(lm_dict.keys())
        keys = sorted(keys)
        self.item_list = []
        for k in keys:
            img_full_path = os.path.join(img_folder, str(k)+'.png')
            assert os.path.exists(
                img_full_path), 'file %s does not exist' % img_full_path
            self.item_list.append((k, img_full_path))
        num_insts = len(self.item_list)
        self.start_ind = worker_ind*(num_insts//worker_num)
        if worker_ind == worker_num-1:
            self.group_len = num_insts - self.start_ind
        else:
            self.group_len = num_insts//worker_num

    def __getitem__(self, index):

        k, img_full_path = self.item_list[self.start_ind:self.start_ind +
                                          self.group_len][index]
        lms = self.lm_dict[k]
        img = cv2.imread(img_full_path)[:, :, ::-1].astype(np.float32)
        return torch.tensor(lms), torch.tensor(img), k

    def __len__(self):
        return self.group_len

      # Build is not required unless generated source files are used
      # - name: import numpy as np
import torch
import torch.nn.functional as F


def photo_loss(pred_img, gt_img, img_mask):
    pred_img = pred_img.float()
    loss = torch.sqrt(torch.sum(torch.square(
        pred_img - gt_img), 3))*img_mask/255
    loss = torch.sum(loss, dim=(1, 2)) / torch.sum(img_mask, dim=(1, 2))
    loss = torch.mean(loss)

    return loss


def lm_loss(pred_lms, gt_lms, weight, img_size=224):
    loss = torch.sum(torch.square(pred_lms/img_size - gt_lms /
                                  img_size), dim=2) * weight.reshape(1, -1)
    loss = torch.mean(loss.sum(1))

    return loss


# def reg_loss(id_coeff, ex_coeff, tex_coeff):

#     loss = torch.square(id_coeff).sum() + \
#         torch.square(tex_coeff).sum() * 1.7e-3 + \
#         torch.square(ex_coeff).sum(1).mean() * 0.8

#     return loss
def get_l2(tensor):
    return torch.square(tensor).sum()


def reflectance_loss(tex, skin_mask):

    skin_mask = skin_mask.unsqueeze(2)
    tex_mean = torch.sum(tex*skin_mask, 1, keepdims=True)/torch.sum(skin_mask)
    loss = torch.sum(torch.square((tex-tex_mean)*skin_mask/255.)) / \
        (tex.shape[0]*torch.sum(skin_mask))

    return loss


def gamma_loss(gamma):

    gamma = gamma.reshape(-1, 3, 9)
    gamma_mean = torch.mean(gamma, dim=1, keepdims=True)
    gamma_loss = torch.mean(torch.square(gamma - gamma_mean))

    return gamma_loss
      #   run: import torch
import torch.nn as nn
import numpy as np
from pytorch3d.structures import Meshes
from pytorch3d.renderer import (
    look_at_view_transform,
    FoVPerspectiveCameras,
    PointLights,
    RasterizationSettings,
    MeshRenderer,
    MeshRasterizer,
    SoftPhongShader,
    TexturesVertex,
    blending
)


class ReconModel(nn.Module):
    def __init__(self, face_model, 
                focal=1015, img_size=224, device='cuda:0'):
        super(ReconModel, self).__init__()
        self.facemodel = face_model

        self.focal = focal
        self.img_size = img_size

        self.device = torch.device(device)

        self.renderer = self.get_renderer(self.device)

        self.kp_inds = torch.tensor(self.facemodel['keypoints']-1).squeeze().long()

        meanshape = nn.Parameter(torch.from_numpy(self.facemodel['meanshape'],).float(), requires_grad=False)
        self.register_parameter('meanshape', meanshape)

        idBase = nn.Parameter(torch.from_numpy(self.facemodel['idBase']).float(), requires_grad=False)
        self.register_parameter('idBase', idBase)

        exBase = nn.Parameter(torch.from_numpy(self.facemodel['exBase']).float(), requires_grad=False)
        self.register_parameter('exBase', exBase)

        meantex = nn.Parameter(torch.from_numpy(self.facemodel['meantex']).float(), requires_grad=False)
        self.register_parameter('meantex', meantex)

        texBase = nn.Parameter(torch.from_numpy(self.facemodel['texBase']).float(), requires_grad=False)
        self.register_parameter('texBase', texBase)

        tri = nn.Parameter(torch.from_numpy(self.facemodel['tri']).float(), requires_grad=False)
        self.register_parameter('tri', tri)

        point_buf = nn.Parameter(torch.from_numpy(self.facemodel['point_buf']).float(), requires_grad=False)
        self.register_parameter('point_buf', point_buf)

    def get_renderer(self, device):
        R, T = look_at_view_transform(10, 0, 0)
        cameras = FoVPerspectiveCameras(device=device, R=R, T=T, znear=0.01, zfar=50,
                                        fov=2*np.arctan(self.img_size//2/self.focal)*180./np.pi)

        lights = PointLights(device=device, location=[[0.0, 0.0, 1e5]], ambient_color=[[1, 1, 1]],
                             specular_color=[[0., 0., 0.]], diffuse_color=[[0., 0., 0.]])

        raster_settings = RasterizationSettings(
            image_size=self.img_size,
            blur_radius=0.0,
            faces_per_pixel=1,
        )
        blend_params = blending.BlendParams(background_color=[0, 0, 0])

        renderer = MeshRenderer(
            rasterizer=MeshRasterizer(
                cameras=cameras,
                raster_settings=raster_settings
            ),
            shader=SoftPhongShader(
                device=device,
                cameras=cameras,
                lights=lights,
                blend_params=blend_params
            )
        )
        return renderer

    def Split_coeff(self, coeff):
        id_coeff = coeff[:, :80]  # identity(shape) coeff of dim 80
        ex_coeff = coeff[:, 80:144]  # expression coeff of dim 64
        tex_coeff = coeff[:, 144:224]  # texture(albedo) coeff of dim 80
        angles = coeff[:, 224:227]  # ruler angles(x,y,z) for rotation of dim 3
        gamma = coeff[:, 227:254]  # lighting coeff for 3 channel SH function of dim 27
        translation = coeff[:, 254:]  # translation coeff of dim 3

        return id_coeff, ex_coeff, tex_coeff, angles, gamma, translation

    def Shape_formation(self, id_coeff, ex_coeff):
        n_b = id_coeff.size(0)

        face_shape = torch.einsum('ij,aj->ai', self.idBase, id_coeff) + \
                     torch.einsum('ij,aj->ai', self.exBase, ex_coeff) + self.meanshape

        face_shape = face_shape.view(n_b, -1, 3)
        face_shape = face_shape - self.meanshape.view(1, -1, 3).mean(dim=1, keepdim=True)

        return face_shape

    def Texture_formation(self, tex_coeff):
        n_b = tex_coeff.size(0)
        face_texture = torch.einsum('ij,aj->ai', self.texBase, tex_coeff) + self.meantex

        face_texture = face_texture.view(n_b, -1, 3)
        return face_texture

    def Compute_norm(self, face_shape):

        face_id = self.tri.long() - 1
        point_id = self.point_buf.long() - 1 
        shape = face_shape
        v1 = shape[:, face_id[:, 0], :]
        v2 = shape[:, face_id[:, 1], :]
        v3 = shape[:, face_id[:, 2], :]
        e1 = v1 - v2
        e2 = v2 - v3
        face_norm = e1.cross(e2)
        empty = torch.zeros((face_norm.size(0), 1, 3), dtype=face_norm.dtype, device=face_norm.device)
        face_norm = torch.cat((face_norm, empty), 1) 
        v_norm = face_norm[:, point_id, :].sum(2)  
        v_norm = v_norm / v_norm.norm(dim=2).unsqueeze(2)

        return v_norm

    def Projection_block(self, face_shape):
        half_image_width = self.img_size // 2
        batchsize = face_shape.shape[0]
        camera_pos = torch.tensor([0.0,0.0,10.0], device=face_shape.device).reshape(1, 1, 3)
        # tensor.reshape(constant([0.0,0.0,10.0]),[1,1,3])
        p_matrix = np.array([self.focal, 0.0, half_image_width, \
                            0.0, self.focal, half_image_width, \
                            0.0, 0.0, 1.0], dtype=np.float32)

        p_matrix = np.tile(p_matrix.reshape(1, 3, 3), [batchsize, 1, 1])
        reverse_z = np.tile(np.reshape(np.array([1.0,0,0,0,1,0,0,0,-1.0], dtype=np.float32),[1,3,3]),
                            [batchsize,1,1])

        p_matrix = torch.tensor(p_matrix, device=face_shape.device)
        reverse_z = torch.tensor(reverse_z, device=face_shape.device)
        face_shape = torch.matmul(face_shape,reverse_z) + camera_pos
        aug_projection = torch.matmul(face_shape,p_matrix.permute((0,2,1)))

        face_projection = aug_projection[:,:,:2]/ \
                        torch.reshape(aug_projection[:,:,2],[batchsize,-1,1])
        return face_projection

    @staticmethod
    def Compute_rotation_matrix(angles):
        n_b = angles.size(0)
        sinx = torch.sin(angles[:, 0])
        siny = torch.sin(angles[:, 1])
        sinz = torch.sin(angles[:, 2])
        cosx = torch.cos(angles[:, 0])
        cosy = torch.cos(angles[:, 1])
        cosz = torch.cos(angles[:, 2])

        rotXYZ = torch.eye(3).view(1, 3, 3).repeat(n_b * 3, 1, 1).view(3, n_b, 3, 3)

        if angles.is_cuda: rotXYZ = rotXYZ.cuda()

        rotXYZ[0, :, 1, 1] = cosx
        rotXYZ[0, :, 1, 2] = -sinx
        rotXYZ[0, :, 2, 1] = sinx
        rotXYZ[0, :, 2, 2] = cosx
        rotXYZ[1, :, 0, 0] = cosy
        rotXYZ[1, :, 0, 2] = siny
        rotXYZ[1, :, 2, 0] = -siny
        rotXYZ[1, :, 2, 2] = cosy
        rotXYZ[2, :, 0, 0] = cosz
        rotXYZ[2, :, 0, 1] = -sinz
        rotXYZ[2, :, 1, 0] = sinz
        rotXYZ[2, :, 1, 1] = cosz

        rotation = rotXYZ[2].bmm(rotXYZ[1]).bmm(rotXYZ[0])

        return rotation.permute(0, 2, 1)

    @staticmethod
    def Rigid_transform_block(face_shape, rotation, translation):
        face_shape_r = torch.matmul(face_shape, rotation)
        face_shape_t = face_shape_r + translation.view(-1, 1, 3)

        return face_shape_t

    @staticmethod
    def Illumination_layer(face_texture, norm, gamma):

        n_b, num_vertex, _ = face_texture.size()
        n_v_full = n_b * num_vertex
        gamma = gamma.view(-1, 3, 9).clone()
        gamma[:, :, 0] += 0.8

        gamma = gamma.permute(0, 2, 1)

        a0 = np.pi
        a1 = 2 * np.pi / np.sqrt(3.0)
        a2 = 2 * np.pi / np.sqrt(8.0)
        c0 = 1 / np.sqrt(4 * np.pi)
        c1 = np.sqrt(3.0) / np.sqrt(4 * np.pi)
        c2 = 3 * np.sqrt(5.0) / np.sqrt(12 * np.pi)
        d0 = 0.5/ np.sqrt(3.0)

        Y0 = torch.ones(n_v_full).to(gamma.device).float() * a0 * c0
        norm = norm.view(-1, 3)
        nx, ny, nz = norm[:, 0], norm[:, 1], norm[:, 2]
        arrH = []

        arrH.append(Y0)
        arrH.append(-a1 * c1 * ny)
        arrH.append(a1 * c1 * nz)
        arrH.append(-a1 * c1 * nx)
        arrH.append(a2 * c2 * nx * ny)
        arrH.append(-a2 * c2 * ny * nz)
        arrH.append(a2 * c2 * d0 * (3 * nz.pow(2) - 1))
        arrH.append(-a2 * c2 * nx * nz)
        arrH.append(a2 * c2 * 0.5 * (nx.pow(2) - ny.pow(2)))

        H = torch.stack(arrH, 1)
        Y = H.view(n_b, num_vertex, 9)
        lighting = Y.bmm(gamma)

        face_color = face_texture * lighting
        return face_color

    def get_lms(self, face_shape, kp_inds):
        lms = face_shape[:, kp_inds, :]
        return lms

    def forward(self, coeff):

        batch_num = coeff.shape[0]

        id_coeff, ex_coeff, tex_coeff, angles, gamma, translation = self.Split_coeff(coeff)

        face_shape = self.Shape_formation(id_coeff, ex_coeff)
        face_texture = self.Texture_formation(tex_coeff)
        face_norm = self.Compute_norm(face_shape)
        rotation = self.Compute_rotation_matrix(angles)
        face_norm_r = face_norm.bmm(rotation)
        face_shape_t = self.Rigid_transform_block(face_shape, rotation, translation)
        face_color = self.Illumination_layer(face_texture, face_norm_r, gamma)
        face_lms_t = self.get_lms(face_shape_t, self.kp_inds)
        lms = self.Projection_block(face_lms_t)
        lms = torch.stack([lms[:, :, 0], self.img_size-lms[:, :, 1]], dim=2)


        face_color = TexturesVertex(face_color)

        tri = self.tri - 1
        mesh = Meshes(face_shape_t, tri.repeat(batch_num, 1, 1), face_color)
        rendered_img = self.renderer(mesh)
        rendered_img = torch.clamp(rendered_img, 0, 255)

        return rendered_img, lms, face_texture, mesh

      - name: import argparse
import os
import torch


class BaseOptions():
    def __init__(self):
        self.parser = argparse.ArgumentParser()
        self.initialized = False

    def initialize(self):
        self.parser.add_argument('--tar_size', type=int, default=256,
                                 help='size for rendering window. We use a square window.')
        self.parser.add_argument('--padding_ratio', type=float, default=0.3,
                                 help='enlarge the face detection bbox by a margin.')
        self.parser.add_argument('--recon_model', type=str, default='bfm09',
                                 help='choose a 3dmm model, default: bfm09')
        self.parser.add_argument('--first_rf_iters', type=int, default=1000,
                                 help='iteration number of rigid fitting for the first frame in video fitting.')
        self.parser.add_argument('--first_nrf_iters', type=int, default=500,
                                 help='iteration number of non-rigid fitting for the first frame in video fitting.')
        self.parser.add_argument('--rest_rf_iters', type=int, default=50,
                                 help='iteration number of rigid fitting for the remaining frames in video fitting.')
        self.parser.add_argument('--rest_nrf_iters', type=int, default=30,
                                 help='iteration number of non-rigid fitting for the remaining frames in video fitting.')
        self.parser.add_argument('--rf_lr', type=float, default=1e-2,
                                 help='learning rate for rigid fitting')
        self.parser.add_argument('--nrf_lr', type=float, default=1e-2,
                                 help='learning rate for non-rigid fitting')
        self.parser.add_argument('--lm_loss_w', type=float, default=100,
                                 help='weight for landmark loss')
        self.parser.add_argument('--rgb_loss_w', type=float, default=1.6,
                                 help='weight for rgb loss')
        self.parser.add_argument('--id_reg_w', type=float, default=1e-3,
                                 help='weight for id coefficient regularizer')
        self.parser.add_argument('--exp_reg_w', type=float, default=0.8e-3,
                                 help='weight for expression coefficient regularizer')
        self.parser.add_argument('--tex_reg_w', type=float, default=1.7e-6,
                                 help='weight for texture coefficient regularizer')
        self.parser.add_argument('--rot_reg_w', type=float, default=1,
                                 help='weight for rotation regularizer')
        self.parser.add_argument('--trans_reg_w', type=float, default=1,
                                 help='weight for translation regularizer')

        self.parser.add_argument('--tex_w', type=float, default=1,
                                 help='weight for texture reflectance loss.')
        self.parser.add_argument('--cache_folder', type=str, default='fitting_cache',
                                 help='path for the cache folder')
        self.parser.add_argument('--nframes_shape', type=int, default=16,
                                 help='number of frames used to estimate shape coefficient in video fitting')
        self.parser.add_argument('--res_folder', type=str, required=True,
                                 help='output path for the image')

        self.initialized = True

    def parse(self):
        if not self.initialized:
            self.initialize()
        self.opt = self.parser.parse_args()

        args = vars(self.opt)

        print('------------ Options -------------')
        for k, v in sorted(args.items()):
            print('%s: %s' % (str(k), str(v)))
        print('-------------- End ----------------')

        return self.opt


class ImageFittingOptions(BaseOptions):
    def initialize(self):
        BaseOptions.initialize(self)
        self.parser.add_argument('--img_path', type=str, required=True,
                                 help='path for the image')
        self.parser.add_argument('--gpu', type=int, default=0,
                                 help='gpu device')


class VideoFittingOptions(BaseOptions):
    def initialize(self):
        BaseOptions.initialize(self)
        self.parser.add_argument('--v_path', type=str, required=True,
                                 help='path for the video')
        self.parser.add_argument('--ngpus', type=int, default=1,
                                 help='gpu device')
        self.parser.add_argument('--nworkers', type=int, default=1,
                                 help='number of workers')
        uses: import pickle
import numpy as np
import os
import torch


def pad_bbox(bbox, img_wh, padding_ratio=0.2):
    x1, y1, x2, y2 = bbox
    width = x2 - x1
    height = y2 - y1
    size_bb = int(max(width, height) * (1+padding_ratio))
    center_x, center_y = (x1 + x2) // 2, (y1 + y2) // 2
    x1 = max(int(center_x - size_bb // 2), 0)
    y1 = max(int(center_y - size_bb // 2), 0)
    size_bb = min(img_wh[0] - x1, size_bb)
    size_bb = min(img_wh[1] - y1, size_bb)

    return [x1, y1, x1+size_bb, y1+size_bb]


def mymkdirs(path):
    if not os.path.exists(path):
        os.makedirs(path)


def get_lm_weights(device):
    w = torch.ones(68).to(device)
    w[28:31] = 10
    w[48:68] = 10
    norm_w = w / w.sum()
    return norm_w


def save_obj(path, v, f, c):
    with open(path, 'w') as file:
        for i in range(len(v)):
            file.write('v %f %f %f %f %f %f\n' %
                       (v[i, 0], v[i, 1], v[i, 2], c[i, 0], c[i, 1], c[i, 2]))

        file.write('\n')

        for i in range(len(f)):
            file.write('f %d %d %d\n' % (f[i, 0], f[i, 1], f[i, 2]))

    file.close()
        # Provide a unique ID to access the sarif output path
        id: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 16
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 16
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,1,2,2,4]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [16]
      dropout: 0.0


data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2
        with: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 4
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 4
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,2,4,4 ]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [ ]
      dropout: 0.0

data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2
          cmakeBuildDirectory: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 3
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 3
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,2,4 ]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [ ]
      dropout: 0.0


data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2
          # Ruleset file that will determine what checks will be run
          ruleset: model:
  base_learning_rate: 4.5e-6
  target: ldm.models.autoencoder.AutoencoderKL
  params:
    monitor: "val/rec_loss"
    embed_dim: 64
    lossconfig:
      target: ldm.modules.losses.LPIPSWithDiscriminator
      params:
        disc_start: 50001
        kl_weight: 0.000001
        disc_weight: 0.5

    ddconfig:
      double_z: True
      z_channels: 64
      resolution: 256
      in_channels: 3
      out_ch: 3
      ch: 128
      ch_mult: [ 1,1,2,2,4,4]  # num_down = len(ch_mult)-1
      num_res_blocks: 2
      attn_resolutions: [16,8]
      dropout: 0.0

data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 12
    wrap: True
    train:
      target: ldm.data.imagenet.ImageNetSRTrain
      params:
        size: 256
        degradation: pil_nearest
    validation:
      target: ldm.data.imagenet.ImageNetSRValidation
      params:
        size: 256
        degradation: pil_nearest

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 1000
        max_images: 8
        increase_log_steps: True

  trainer:
    benchmark: True
    accumulate_grad_batches: 2

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: model:
  base_learning_rate: 2.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    image_size: 64
    channels: 3
    monitor: val/loss_simple_ema

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 224
        attention_resolutions:
        # note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 64 for f4
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        num_head_channels: 32
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 3
        n_embed: 8192
        ckpt_path: models/first_stage_models/vq-f4/model.ckpt
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config: __is_unconditional__
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 48
    num_workers: 5
    wrap: false
    train:
      target: taming.data.faceshq.CelebAHQTrain
      params:
        size: 256
    validation:
      target: taming.data.faceshq.CelebAHQValidation
      params:
        size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True
        uses: model:
  base_learning_rate: 1.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    cond_stage_key: class_label
    image_size: 32
    channels: 4
    cond_stage_trainable: true
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32
        in_channels: 4
        out_channels: 4
        model_channels: 256
        attention_resolutions:
        #note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 32 for f8
        - 4
        - 2
        - 1
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 4
        num_head_channels: 32
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 512
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 4
        n_embed: 16384
        ckpt_path: configs/first_stage_models/vq-f8/model.yaml
        ddconfig:
          double_z: false
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions:
          - 32
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config:
      target: ldm.modules.encoders.modules.ClassEmbedder
      params:
        embed_dim: 512
        key: class_label
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 64
    num_workers: 12
    wrap: false
    train:
      target: ldm.data.imagenet.ImageNetTrain
      params:
        config:
          size: 256
    validation:
      target: ldm.data.imagenet.ImageNetValidation
      params:
        config:
          size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True
        with: model:
  base_learning_rate: 0.0001
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    cond_stage_key: class_label
    image_size: 64
    channels: 3
    cond_stage_trainable: true
    conditioning_key: crossattn
    monitor: val/loss
    use_ema: False

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 192
        attention_resolutions:
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 5
        num_heads: 1
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 512

    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 3
        n_embed: 8192
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config:
      target: ldm.modules.encoders.modules.ClassEmbedder
      params:
        n_classes: 1001
        embed_dim: 512
        key: class_label
          sarif_file: model:
  base_learning_rate: 2.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    image_size: 64
    channels: 3
    monitor: val/loss_simple_ema
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 224
        attention_resolutions:
        # note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 64 for f4
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        num_head_channels: 32
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        embed_dim: 3
        n_embed: 8192
        ckpt_path: configs/first_stage_models/vq-f4/model.yaml
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config: __is_unconditional__
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 42
    num_workers: 5
    wrap: false
    train:
      target: taming.data.faceshq.FFHQTrain
      params:
        size: 256
    validation:
      target: taming.data.faceshq.FFHQValidation
      params:
        size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True

      # Upload SARIF file as an Artifact to download and view
      # - name: model:
  base_learning_rate: 2.0e-06
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0195
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    image_size: 64
    channels: 3
    monitor: val/loss_simple_ema
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 64
        in_channels: 3
        out_channels: 3
        model_channels: 224
        attention_resolutions:
        # note: this isn\t actually the resolution but
        # the downsampling factor, i.e. this corresnponds to
        # attention on spatial resolution 8,16,32, as the
        # spatial reolution of the latents is 64 for f4
        - 8
        - 4
        - 2
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        num_head_channels: 32
    first_stage_config:
      target: ldm.models.autoencoder.VQModelInterface
      params:
        ckpt_path: configs/first_stage_models/vq-f4/model.yaml
        embed_dim: 3
        n_embed: 8192
        ddconfig:
          double_z: false
          z_channels: 3
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config: __is_unconditional__
data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 48
    num_workers: 5
    wrap: false
    train:
      target: ldm.data.lsun.LSUNBedroomsTrain
      params:
        size: 256
    validation:
      target: ldm.data.lsun.LSUNBedroomsValidation
      params:
        size: 256


lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False

  trainer:
    benchmark: True
      #   uses: model:
  base_learning_rate: 5.0e-5   # set to target_lr by starting main.py with '--scale_lr False'
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.0155
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    loss_type: l1
    first_stage_key: "image"
    cond_stage_key: "image"
    image_size: 32
    channels: 4
    cond_stage_trainable: False
    concat_mode: False
    scale_by_std: True
    monitor: 'val/loss_simple_ema'

    scheduler_config: # 10000 warmup steps
      target: ldm.lr_scheduler.LambdaLinearScheduler
      params:
        warm_up_steps: [10000]
        cycle_lengths: [10000000000000]
        f_start: [1.e-6]
        f_max: [1.]
        f_min: [ 1.]

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32
        in_channels: 4
        out_channels: 4
        model_channels: 192
        attention_resolutions: [ 1, 2, 4, 8 ]   # 32, 16, 8, 4
        num_res_blocks: 2
        channel_mult: [ 1,2,2,4,4 ]  # 32, 16, 8, 4, 2
        num_heads: 8
        use_scale_shift_norm: True
        resblock_updown: True

    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        embed_dim: 4
        monitor: "val/rec_loss"
        ckpt_path: "models/first_stage_models/kl-f8/model.ckpt"
        ddconfig:
          double_z: True
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult: [ 1,2,4,4 ]  # num_down = len(ch_mult)-1
          num_res_blocks: 2
          attn_resolutions: [ ]
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config: "__is_unconditional__"

data:
  target: main.DataModuleFromConfig
  params:
    batch_size: 96
    num_workers: 5
    wrap: False
    train:
      target: ldm.data.lsun.LSUNChurchesTrain
      params:
        size: 256
    validation:
      target: ldm.data.lsun.LSUNChurchesValidation
      params:
        size: 256

lightning:
  callbacks:
    image_logger:
      target: main.ImageLogger
      params:
        batch_frequency: 5000
        max_images: 8
        increase_log_steps: False


  trainer:
    benchmark: True
      #   with: model:
  base_learning_rate: 5.0e-05
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.00085
    linear_end: 0.012
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: image
    cond_stage_key: caption
    image_size: 32
    channels: 4
    cond_stage_trainable: true
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    scale_factor: 0.18215
    use_ema: False

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32
        in_channels: 4
        out_channels: 4
        model_channels: 320
        attention_resolutions:
        - 4
        - 2
        - 1
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 4
        - 4
        num_heads: 8
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 1280
        use_checkpoint: true
        legacy: False

    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        embed_dim: 4
        monitor: val/rec_loss
        ddconfig:
          double_z: true
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config:
      target: ldm.modules.encoders.modules.BERTEmbedder
      params:
        n_embed: 1280
        n_layer: 32
      #     name: model:
  base_learning_rate: 0.0001
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.0015
    linear_end: 0.015
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: jpg
    cond_stage_key: nix
    image_size: 48
    channels: 16
    cond_stage_trainable: false
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    scale_by_std: false
    scale_factor: 0.22765929
    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 48
        in_channels: 16
        out_channels: 16
        model_channels: 448
        attention_resolutions:
        - 4
        - 2
        - 1
        num_res_blocks: 2
        channel_mult:
        - 1
        - 2
        - 3
        - 4
        use_scale_shift_norm: false
        resblock_updown: false
        num_head_channels: 32
        use_spatial_transformer: true
        transformer_depth: 1
        context_dim: 768
        use_checkpoint: true
    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        monitor: val/rec_loss
        embed_dim: 16
        ddconfig:
          double_z: true
          z_channels: 16
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 1
          - 2
          - 2
          - 4
          num_res_blocks: 2
          attn_resolutions:
          - 16
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity
    cond_stage_config:
      target: torch.nn.Identity
      #     path: model:
  base_learning_rate: 1.0e-04
  target: ldm.models.diffusion.ddpm.LatentDiffusion
  params:
    linear_start: 0.00085
    linear_end: 0.0120
    num_timesteps_cond: 1
    log_every_t: 200
    timesteps: 1000
    first_stage_key: "jpg"
    cond_stage_key: "txt"
    image_size: 64
    channels: 4
    cond_stage_trainable: false   # Note: different from the one we trained before
    conditioning_key: crossattn
    monitor: val/loss_simple_ema
    scale_factor: 0.18215
    use_ema: False

    scheduler_config: # 10000 warmup steps
      target: ldm.lr_scheduler.LambdaLinearScheduler
      params:
        warm_up_steps: [ 10000 ]
        cycle_lengths: [ 10000000000000 ] # incredibly large number to prevent corner cases
        f_start: [ 1.e-6 ]
        f_max: [ 1. ]
        f_min: [ 1. ]

    unet_config:
      target: ldm.modules.diffusionmodules.openaimodel.UNetModel
      params:
        image_size: 32 # unused
        in_channels: 4
        out_channels: 4
        model_channels: 320
        attention_resolutions: [ 4, 2, 1 ]
        num_res_blocks: 2
        channel_mult: [ 1, 2, 4, 4 ]
        num_heads: 8
        use_spatial_transformer: True
        transformer_depth: 1
        context_dim: 768
        use_checkpoint: True
        legacy: False

    first_stage_config:
      target: ldm.models.autoencoder.AutoencoderKL
      params:
        embed_dim: 4
        monitor: val/rec_loss
        ddconfig:
          double_z: true
          z_channels: 4
          resolution: 256
          in_channels: 3
          out_ch: 3
          ch: 128
          ch_mult:
          - 1
          - 2
          - 4
          - 4
          num_res_blocks: 2
          attn_resolutions: []
          dropout: 0.0
        lossconfig:
          target: torch.nn.Identity

    cond_stage_config:
      target: ldm.modules.encoders.modules.FrozenCLIPEmbedder
