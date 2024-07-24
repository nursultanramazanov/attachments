# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: ﻿using GHelper.Gpu.AMD;
using GHelper.Helpers;
using GHelper.Input;
using GHelper.Mode;
using GHelper.USB;
using HidSharp;
using System.Text;



namespace GHelper.Ally
{

    public enum ControllerMode : int
    {
        Auto = 0,
        Gamepad = 1,
        WASD = 2,
        Mouse = 3,
        Skip = -1,
    }

    public enum BindingZone : byte
    {
        DPadUpDown = 1,
        DPadLeftRight = 2,
        StickClick = 3,
        Bumper = 4,
        AB = 5,
        XY = 6,
        ViewMenu = 7,
        M1M2 = 8,
        Trigger = 9
    }

    public class AllyControl
    {
        static System.Timers.Timer timer = default!;
        static AmdGpuControl amdControl = new AmdGpuControl();

        SettingsForm settings;

        static ControllerMode _mode = ControllerMode.Auto;
        static ControllerMode _applyMode = ControllerMode.Mouse;

        static int _autoCount = 0;

        static int _upCount = 0;
        static int _downCount = 0;

        static int tdpMin = 6;
        static int tdpStable = tdpMin;
        static int tdpCurrent = -1;

        static bool autoTDP = false;

        static int fpsLimit = -1;


        public const string BindA = "01-01";
        public const string BindB = "01-02";
        public const string BindX = "01-03";
        public const string BindY = "01-04";
        public const string BindLB = "01-05";
        public const string BindRB = "01-06";
        public const string BindLS = "01-07";
        public const string BindRS = "01-08";
        public const string BindDU = "01-09";
        public const string BindDD = "01-0A";
        public const string BindDL = "01-0B";
        public const string BindDR = "01-0C";
        public const string BindVB = "01-11";
        public const string BindMB = "01-12";
        public const string BindM1 = "02-8F";
        public const string BindM2 = "02-8E";
        public const string BindLT = "01-0D";
        public const string BindRT = "01-0E";
        public const string BindXB = "01-13";

        public const string BindMouseL = "03-01";
        public const string BindMouseR = "03-02";

        public const string BindKBU = "02-98";
        public const string BindKBD = "02-99";
        public const string BindKBL = "02-9A";
        public const string BindKBR = "02-9B";

        public const string BindTab = "02-0D";
        public const string BindEnter = "02-5A";
        public const string BindBack = "02-66";
        public const string BindEsc = "02-76";

        public const string BindPgU = "02-96";
        public const string BindPgD = "02-97";

        public const string BindShift = "02-88";
        public const string BindCtrl = "02-8C";
        public const string BindAlt = "02-8A";
        public const string BindWin = "02-82";

        public const string BindTaskManager = "04-03-8C-88-76";
        public const string BindCloseWindow = "04-02-8A-0C";

        public const string BindBrightnessDown = "04-04-8C-88-8A-05";
        public const string BindBrightnessUp = "04-04-8C-88-8A-06";
        public const string BindXGM = "04-04-8C-88-8A-04";
        public const string BindToggleMode = "04-04-8C-88-8A-0C";

        public const string BindOverlay = "04-03-8C-88-44";

        public const string BindShiftTab = "04-02-88-0D";
        public const string BindAltTab = "04-02-8A-0D";

        public const string BindVolUp = "05-03";
        public const string BindVolDown = "05-02";

        public const string BindPrintScrn = "02-C3";

        public const string BindScreenshot = "04-03-82-88-1B";
        public const string BindShowDesktop = "04-02-82-23";

        public const string BindShowKeyboard = "05-19";

        static byte[] CommandReady = new byte[] { AsusHid.INPUT_ID, 0xD1, 0x0A, 0x01 };
        static byte[] CommandSave = new byte[] { AsusHid.INPUT_ID, 0xD1, 0x0F, 0x20 };

        public static Dictionary<string, string> BindCodes = new Dictionary<string, string>
        {
            { "", "--------" },
            { "00-00", "[ Disabled ]" },

            { BindM1, "M1" },
            { BindM2, "M2" },

            { BindA, "A" },
            { BindB, "B" },

            { BindX, "X" },
            { BindY, "Y" },

            { BindLB, "Left Bumper" },
            { BindRB, "Right Bumper" },

            { BindLS, "Left Stick Click" },
            { BindRS, "Right Stick Click" },

            { BindDU, "DPad Up" },
            { BindDD, "DPad Down" },

            { BindDL, "DPad Left" },
            { BindDR, "DPad Right" },

            { BindVB, "View Button" },
            { BindMB, "Menu Button" },

            { BindXB, "XBox/Steam" },

            { BindToggleMode, "Controller Mode" },

            { BindVolUp, "Vol Up" },
            { BindVolDown, "Vol Down" },
            { BindBrightnessUp, "Bright Up" },
            { BindBrightnessDown, "Bright Down" },

            { BindShowKeyboard, "Show Keyboard" },
            { BindShowDesktop, "Show Desktop" },
            { BindScreenshot, "Screenshot" },

            { BindOverlay, "AMD Overlay" },
            { BindTaskManager, "Task Manager" },
            { BindCloseWindow, "Close Window" },
            { BindShiftTab, "Shift-Tab" },
            { BindAltTab, "Alt-Tab" },
            { BindXGM, "XGM Toggle" },


            { BindEsc, "Esc" },
            { BindBack, "Backspace" },
            { BindTab, "Tab" },
            { BindEnter, "Enter" },
            { BindShift, "LShift" },
            { BindAlt, "LAlt" },
            { BindCtrl, "LCtl" },
            { BindWin, "WIN" },
            { BindPrintScrn, "PrntScn" },

            { BindPgU, "PgUp" },
            { BindPgD, "PgDwn" },
            { BindKBU, "UpArrow" },
            { BindKBD, "DownArrow" },
            { BindKBL, "LeftArrow" },
            { BindKBR, "RightArrow" },

            { "02-05", "F1" },
            { "02-06", "F2" },
            { "02-04", "F3" },
            { "02-0C", "F4" },
            { "02-03", "F5" },
            { "02-0B", "F6" },
            { "02-80", "F7" },
            { "02-0A", "F8" },
            { "02-01", "F9" },
            { "02-09", "F10" },
            { "02-78", "F11" },
            { "02-07", "F12" },
            { "02-0E", "`" },
            { "02-16", "1" },
            { "02-1E", "2" },
            { "02-26", "3" },
            { "02-25", "4" },
            { "02-2E", "5" },
            { "02-36", "6" },
            { "02-3D", "7" },
            { "02-3E", "8" },
            { "02-46", "9" },
            { "02-45", "0" },
            { "02-4E", "-" },
            { "02-55", "=" },
            { "02-15", "Q" },
            { "02-1D", "W" },
            { "02-24", "E" },
            { "02-2D", "R" },
            { "02-2C", "T" },
            { "02-35", "Y" },
            { "02-3C", "U" },
            { "02-44", "O" },
            { "02-4D", "P" },
            { "02-54", "[" },
            { "02-5B", "]" },
            { "02-5D", "|" },
            { "02-58", "Caps" },
            { "02-1C", "A" },
            { "02-1B", "S" },
            { "02-23", "D" },
            { "02-2B", "F" },
            { "02-34", "G" },
            { "02-33", "H" },
            { "02-3B", "J" },
            { "02-42", "k" },
            { "02-4B", "l" },
            { "02-4C", ";" },
            { "02-52", "'" },
            { "02-22", "X" },
            { "02-1A", "Z" },
            { "02-21", "C" },
            { "02-2A", "V" },
            { "02-32", "B" },
            { "02-31", "N" },
            { "02-3A", "M" },
            { "02-41", "," },
            { "02-49", "." },
            { "02-89", "RShift" },
            { "02-29", "Space" },
            { "02-8B", "RAlt" },
            { "02-84", "App menu" },
            { "02-8D", "RCtl" },
            { "02-7E", "ScrLk" },
            { "02-C2", "Insert" },
            { "02-C0", "Delete" },
            { "02-94", "Home" },
            { "02-95", "End" },
            { "02-77", "NumLock" },
            { "02-90", "NumSlash" },
            { "02-7C", "NumStar" },
            { "02-7B", "NumHyphen" },
            { "02-70", "Num0" },
            { "02-69", "Num1" },
            { "02-72", "Num2" },
            { "02-7A", "Num3" },
            { "02-6B", "Num4" },
            { "02-73", "Num5" },
            { "02-74", "Num6" },
            { "02-6C", "Num7" },
            { "02-75", "Num8" },
            { "02-7D", "Num9" },
            { "02-79", "NumPlus" },
            { "02-81", "NumEnter" },
            { "02-71", "NumPeriod" },

            { BindMouseL, "Mouse left click" },
            { BindMouseR, "Mouse right click" },
            { "03-03", "Mouse middle click" },
            { "03-04", "Mouse scroll up" },
            { "03-05", "Mouse scroll down" },

            //{ "05-16", "Screenshot" },
            //{ "05-1C", "Show desktop" },

            { "05-1E", "Begin recording" },
            { "05-01", "Mic off" },

        };

        public AllyControl(SettingsForm settingsForm)
        {
            if (!AppConfig.IsAlly()) return;
            settings = settingsForm;

            if (timer is null)
            {
                timer = new System.Timers.Timer(300);
                timer.Elapsed += Timer_Elapsed;
                Logger.WriteLine("Ally timer");
            }
        }

        private int GetMaxTDP()
        {
            int tdp = AppConfig.GetMode("limit_total");
            if (tdp > 0) return tdp;
            switch (Modes.GetCurrentBase())
            {
                case 1:
                    return 25;
                case 2:
                    return 10;
                default:
                    return 15;
            }
        }

        private int GetTDP()
        {
            if (tdpCurrent < 0) tdpCurrent = GetMaxTDP();
            return tdpCurrent;
        }

        private void SetTDP(int tdp, string log)
        {
            if (tdp < tdpStable) tdp = tdpStable;

            int max = GetMaxTDP();
            if (tdp > max) tdp = max;

            if (tdp == tdpCurrent) return;
            if (!autoTDP) return;

            Program.acpi.DeviceSet(AsusACPI.PPT_APUA0, tdp, log);
            Program.acpi.DeviceSet(AsusACPI.PPT_APUA3, tdp, null);
            Program.acpi.DeviceSet(AsusACPI.PPT_APUC1, tdp, null);

            tdpCurrent = tdp;
        }

        private void Timer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            if (!autoTDP && _mode != ControllerMode.Auto) return;

            float fps = amdControl.GetFPS();

            if (autoTDP && fpsLimit > 0 && fpsLimit <= 120)
            {
                int power = (int)amdControl.GetGpuPower();
                //Debug.WriteLine($"{power}: {fps}");

                if (fps <= Math.Min(fpsLimit * 0.9, fpsLimit - 4)) _upCount++;
                else _upCount = 0;

                if (fps >= Math.Min(fpsLimit * 0.95, fpsLimit - 2)) _downCount++;
                else _downCount = 0;

                var tdp = GetTDP();
                if (_upCount >= 1)
                {
                    _downCount = 0;
                    _upCount = 0;
                    SetTDP(tdp + 1, $"AutoTDP+ [{power}]{fps}");
                }

                if (_downCount >= 8 && power < tdp)
                {
                    _upCount = 0;
                    _downCount--;
                    SetTDP(tdp - 1, $"AutoTDP- [{power}]{fps}");
                }
            }

            if (_mode == ControllerMode.Auto)
            {
                ControllerMode newMode = (fps > 0) ? ControllerMode.Gamepad : ControllerMode.Mouse;

                if (_applyMode != newMode) _autoCount++;
                else _autoCount = 0;

                if (_autoCount == 3)
                {
                    _autoCount = 0;
                    ApplyMode(newMode);
                    Logger.WriteLine($"Controller Mode {fps}: {newMode}");
                }
            }

        }

        public void ToggleAutoTDP()
        {
            autoTDP = !autoTDP;
            tdpCurrent = -1;

            if (!autoTDP)
            {
                Program.modeControl.SetPerformanceMode();
            }

            settings.VisualiseAutoTDP(autoTDP);

        }

        public void Init()
        {
            if (AppConfig.IsAlly()) settings.VisualiseAlly(true);
            else return;

            SetMode((ControllerMode)AppConfig.Get("controller_mode", (int)ControllerMode.Auto), true);

            settings.VisualiseBacklight(InputDispatcher.GetBacklight());

            fpsLimit = amdControl.GetFPSLimit();
            settings.VisualiseFPSLimit(fpsLimit);
        }

        public void ToggleFPSLimit()
        {
            switch (fpsLimit)
            {
                case 30:
                    fpsLimit = 40;
                    break;
                case 40:
                    fpsLimit = 45;
                    break;
                case 45:
                    fpsLimit = 50;
                    break;
                case 50:
                    fpsLimit = 60;
                    break;
                case 60:
                    fpsLimit = 90;
                    break;
                case 90:
                    fpsLimit = 120;
                    break;
                case 120:
                    fpsLimit = 240;
                    break;
                default:
                    fpsLimit = 30;
                    break;
            }

            int result = amdControl.SetFPSLimit(fpsLimit);
            Logger.WriteLine($"FPS Limit {fpsLimit}: {result}");

            settings.VisualiseFPSLimit(fpsLimit);

        }


        public void ToggleBacklight()
        {
            InputDispatcher.SetBacklight(4, true);
            settings.VisualiseBacklight(InputDispatcher.GetBacklight());
        }

        static private byte[] DecodeBinding(string binding = "")
        {
            byte[] bytes;

            if (binding == "" || binding is null) return new byte[2];

            try
            {
                bytes = AppConfig.StringToBytes(binding);
            }
            catch
            {
                return new byte[2];
            }

            byte[] code = new byte[10];
            code[0] = bytes[0];

            switch (bytes[0])
            {
                case 0x02:
                    code[2] = bytes[1];
                    break;
                case 0x03:
                    code[4] = bytes[1];
                    break;
                case 0x04:
                    bytes.Skip(1).ToArray().CopyTo(code, 5);
                    break;
                case 0x05:
                    code[3] = bytes[1];
                    break;
                default:
                    code[1] = bytes[1];
                    break;
            }

            return code;
        }

        static private void BindZone(BindingZone zone)
        {
            string KeyL1, KeyR1;
            string KeyL2, KeyR2;

            bool desktop = (_applyMode == ControllerMode.Mouse);

            switch (zone)
            {
                case BindingZone.DPadUpDown:
                    KeyL1 = AppConfig.GetString("bind_du", desktop ? BindKBU : BindDU);
                    KeyR1 = AppConfig.GetString("bind_dd", desktop ? BindKBD : BindDD);
                    KeyL2 = AppConfig.GetString("bind2_du", BindShowKeyboard);
                    KeyR2 = AppConfig.GetString("bind2_dd", BindShowDesktop);
                    break;
                case BindingZone.DPadLeftRight:
                    KeyL1 = AppConfig.GetString("bind_dl", desktop ? BindKBL : BindDL);
                    KeyR1 = AppConfig.GetString("bind_dr", desktop ? BindKBR : BindDR);
                    KeyL2 = AppConfig.GetString("bind2_dl", BindBrightnessDown);
                    KeyR2 = AppConfig.GetString("bind2_dr", BindBrightnessUp);
                    break;
                case BindingZone.StickClick:
                    KeyL1 = AppConfig.GetString("bind_ls", desktop ? BindShift : BindLS);
                    KeyR1 = AppConfig.GetString("bind_rs", desktop ? BindMouseL : BindRS);
                    KeyL2 = AppConfig.GetString("bind2_ls");
                    KeyR2 = AppConfig.GetString("bind2_rs", BindToggleMode);
                    break;
                case BindingZone.Bumper:
                    KeyL1 = AppConfig.GetString("bind_lb", desktop ? BindTab : BindLB);
                    KeyR1 = AppConfig.GetString("bind_rb", desktop ? BindMouseL : BindRB);
                    KeyL2 = AppConfig.GetString("bind2_lb");
                    KeyR2 = AppConfig.GetString("bind2_rb");
                    break;
                case BindingZone.AB:
                    KeyL1 = AppConfig.GetString("bind_a", desktop ? BindEnter : BindA);
                    KeyR1 = AppConfig.GetString("bind_b", desktop ? BindEsc : BindB);
                    KeyL2 = AppConfig.GetString("bind2_a");
                    KeyR2 = AppConfig.GetString("bind2_b");
                    break;
                case BindingZone.XY:
                    KeyL1 = AppConfig.GetString("bind_x", desktop ? BindPgD : BindX);
                    KeyR1 = AppConfig.GetString("bind_y", desktop ? BindPgU : BindY);
                    KeyL2 = AppConfig.GetString("bind2_x", BindScreenshot);
                    KeyR2 = AppConfig.GetString("bind2_y", BindOverlay);
                    break;
                case BindingZone.ViewMenu:
                    KeyL1 = AppConfig.GetString("bind_vb", BindVB);
                    KeyR1 = AppConfig.GetString("bind_mb", BindMB);
                    KeyL2 = AppConfig.GetString("bind2_vb");
                    KeyR2 = AppConfig.GetString("bind2_mb");
                    break;
                case BindingZone.M1M2:
                    KeyL1 = AppConfig.GetString("bind_m2", BindM2);
                    KeyR1 = AppConfig.GetString("bind_m1", BindM1);
                    KeyL2 = AppConfig.GetString("bind2_m2", BindM2);
                    KeyR2 = AppConfig.GetString("bind2_m1", BindM1);
                    break;
                default:
                    KeyL1 = AppConfig.GetString("bind_trl", desktop ? BindShiftTab : BindLT);
                    KeyR1 = AppConfig.GetString("bind_trr", desktop ? BindMouseR : BindRT);
                    KeyL2 = AppConfig.GetString("bind2_trl");
                    KeyR2 = AppConfig.GetString("bind2_trr");
                    break;
            }

            if (KeyL1 == "" && KeyR1 == "") return;

            byte[] bindings = new byte[50];
            byte[] init = new byte[] { AsusHid.INPUT_ID, 0xd1, 0x02, (byte)zone, 0x2c };

            init.CopyTo(bindings, 0);

            DecodeBinding(KeyL1).CopyTo(bindings, 5);
            DecodeBinding(KeyL2).CopyTo(bindings, 16);

            DecodeBinding(KeyR1).CopyTo(bindings, 27);
            DecodeBinding(KeyR2).CopyTo(bindings, 38);

            //AsusHid.WriteInput(CommandReady, null);
            AsusHid.WriteInput(bindings, null);

        }

        static void WakeUp()
        {
            AsusHid.WriteInput(Encoding.ASCII.GetBytes("ZASUS Tech.Inc."), "Init");
        }

        static public void SetDeadzones()
        {
            AsusHid.WriteInput(new byte[] { AsusHid.INPUT_ID, 0xd1, 4, 4,
                (byte)AppConfig.Get("ls_min", 0),
                (byte)AppConfig.Get("ls_max", 100),
                (byte)AppConfig.Get("rs_min", 0),
                (byte)AppConfig.Get("rs_max", 100)
            }, null);

            AsusHid.WriteInput(new byte[] { AsusHid.INPUT_ID, 0xd1, 5, 4,
                (byte)AppConfig.Get("lt_min", 0),
                (byte)AppConfig.Get("lt_max", 100),
                (byte)AppConfig.Get("rt_min", 0),
                (byte)AppConfig.Get("rt_max", 100)
            }, null);

            AsusHid.WriteInput(new byte[] { AsusHid.INPUT_ID, 0xd1, 6, 2,
                (byte)AppConfig.Get("vibra", 100),
                (byte)AppConfig.Get("vibra", 100)
            }, null);

        }

        public static void ApplyXBoxStatus()
        {
            AsusHid.WriteInput(new byte[] { AsusHid.INPUT_ID, 0xD1, 0x0B, 0x01, AppConfig.Is("controller_disabled") ? (byte)0x02 : (byte)0x01 }, "Status");
        }

        public static void ApplyMode(ControllerMode applyMode = ControllerMode.Auto, bool init = false)
        {
            Task.Run(() =>
            {

                if (applyMode == ControllerMode.Skip) return;

                HidStream? input = AsusHid.FindHidStream(AsusHid.INPUT_ID);
                int count = 0;

                while (input == null && count++ < 10)
                {
                    input = AsusHid.FindHidStream(AsusHid.INPUT_ID);
                    Thread.Sleep(500);
                }

                if (input == null)
                {
                    Logger.WriteLine($"Controller not found");
                    return;
                }

                if (applyMode != ControllerMode.Auto) _applyMode = applyMode;

                if (init)
                {
                    WakeUp();
                    InputDispatcher.SetBacklightAuto(true);
                }

                AsusHid.WriteInput(new byte[] { AsusHid.INPUT_ID, 0xD1, 0x01, 0x01, (byte)_applyMode }, "Controller");
                //AsusHid.WriteInput(CommandSave, null);

                BindZone(BindingZone.M1M2);
                BindZone(BindingZone.DPadUpDown);
                BindZone(BindingZone.DPadLeftRight);
                BindZone(BindingZone.StickClick);
                BindZone(BindingZone.Bumper);
                BindZone(BindingZone.AB);
                BindZone(BindingZone.XY);
                BindZone(BindingZone.ViewMenu);
                BindZone(BindingZone.Trigger);

                AsusHid.WriteInput(CommandSave, null);

                SetDeadzones();

            });
        }

        private void SetMode(ControllerMode mode, bool init = false)
        {

            _mode = mode;
            AppConfig.Set("controller_mode", (int)mode);

            amdControl.StopFPS();
            ApplyMode(mode, init);

            amdControl.StartFPS();
            timer.Start();

            settings.VisualiseController(mode);
        }


        public void ToggleModeHotkey()
        {
            if (_applyMode == ControllerMode.Gamepad)
            {
                SetMode(ControllerMode.Mouse);
                Program.toast.RunToast("Mouse", ToastIcon.Controller);
            }
            else
            {
                SetMode(ControllerMode.Gamepad);
                Program.toast.RunToast("Gamepad", ToastIcon.Controller);
            }
        }

        public void ToggleMode()
        {
            switch (_mode)
            {
                case ControllerMode.Auto:
                    SetMode(ControllerMode.Gamepad);
                    break;
                case ControllerMode.Gamepad:
                    SetMode(ControllerMode.Mouse);
                    break;
                case ControllerMode.Mouse:
                    SetMode(ControllerMode.Skip);
                    break;
                case ControllerMode.Skip:
                    SetMode(ControllerMode.Auto);
                    break;
            }
        }

    }
}
  push: namespace GHelper.AnimeMatrix.Communication.Platform
{
    public abstract class UsbProvider : IDisposable
    {
        protected ushort VendorID { get; }
        protected ushort ProductID { get; }

        protected UsbProvider(ushort vendorId, ushort productId)
        {
            VendorID = vendorId;
            ProductID = productId;
        }

        public abstract void Set(byte[] data);
        public abstract byte[] Get(byte[] data);
        public abstract void Read(byte[] data);
        public abstract void Write(byte[] data);

        public abstract void Dispose();
    }
}
    branches: [ "main" ]
  pull_request: using System.ComponentModel;
using HidSharp;

namespace GHelper.AnimeMatrix.Communication.Platform
{
    internal class WindowsUsbProvider : UsbProvider
    {
        protected HidDevice HidDevice { get; }
        protected HidStream HidStream { get; }

        public WindowsUsbProvider(ushort vendorId, ushort productId, string path, int timeout = 500) : base(vendorId, productId)
        {
            try
            {
                HidDevice = DeviceList.Local.GetHidDevices(vendorId, productId)
                   .First(x => x.DevicePath.Contains(path));
            }
            catch
            {
                throw new IOException("HID device was not found on your machine.");
            }

            var config = new OpenConfiguration();
            config.SetOption(OpenOption.Interruptible, true);
            config.SetOption(OpenOption.Exclusive, false);
            config.SetOption(OpenOption.Priority, 10);
            HidStream = HidDevice.Open(config);
            HidStream.ReadTimeout = timeout;
            HidStream.WriteTimeout = timeout;
        }

        public WindowsUsbProvider(ushort vendorId, ushort productId, int maxFeatureReportLength)
            : base(vendorId, productId)
        {
            try
            {
                HidDevice = DeviceList.Local
                    .GetHidDevices(vendorId, productId)
                    .First(x => x.GetMaxFeatureReportLength() >= maxFeatureReportLength);
                Logger.WriteLine("Matrix Device: " + HidDevice.DevicePath + " " + HidDevice.GetMaxFeatureReportLength());
            }
            catch
            {
                throw new IOException("Matrix control device was not found on your machine.");
            }

            var config = new OpenConfiguration();
            config.SetOption(OpenOption.Interruptible, true);
            config.SetOption(OpenOption.Exclusive, false);
            config.SetOption(OpenOption.Priority, 10);

            HidStream = HidDevice.Open(config);
        }

        public override void Set(byte[] data)
        {
            WrapException(() =>
            {
                HidStream.SetFeature(data);
                HidStream.Flush();
            });
        }

        public override byte[] Get(byte[] data)
        {
            var outData = new byte[data.Length];
            Array.Copy(data, outData, data.Length);

            WrapException(() =>
            {
                HidStream.GetFeature(outData);
                HidStream.Flush();
            });

            return data;
        }

        public override void Read(byte[] data)
        {
            WrapException(() =>
            {
                HidStream.Read(data);
            });
        }

        public override void Write(byte[] data)
        {
            WrapException(() =>
            {
                HidStream.Write(data);
                HidStream.Flush();
            });
        }

        public override void Dispose()
        {
            HidStream.Dispose();
        }

        private void WrapException(Action action)
        {
            try
            {
                action();
            }
            catch (IOException e)
            {
                if (e.InnerException is Win32Exception w32e)
                {
                    if (w32e.NativeErrorCode != 0)
                    {
                        throw;
                    }
                }
            }
        }
    }
}
    branches: [ "main" ]
  schedule: ﻿// Source thanks to https://github.com/vddCore/Starlight :)

using GHelper.AnimeMatrix.Communication.Platform;

namespace GHelper.AnimeMatrix.Communication
{
    public abstract class Device : IDisposable
    {
        protected UsbProvider? _usbProvider;

        protected ushort _vendorId;
        protected ushort _productId;
        protected int _maxFeatureReportLength;

        protected Device(ushort vendorId, ushort productId)
        {
            _vendorId = vendorId;
            _productId = productId;
        }

        protected Device(ushort vendorId, ushort productId, int maxFeatureReportLength)
        {
            _vendorId = vendorId;
            _productId = productId;
            _maxFeatureReportLength = maxFeatureReportLength;
            SetProvider();
        }

        public ushort VendorID()
        {
            return _vendorId;
        }

        public ushort ProductID()
        {
            return _productId;
        }

        public virtual void SetProvider()
        {
            _usbProvider = new WindowsUsbProvider(_vendorId, _productId, _maxFeatureReportLength);
        }

        protected T Packet<T>(params byte[] command) where T : Packet
        {
            return (T)Activator.CreateInstance(typeof(T), command)!;
        }

        public void Set(Packet packet)
            => _usbProvider?.Set(packet.Data);

        public byte[] Get(Packet packet)
            => _usbProvider?.Get(packet.Data);

        public void Read(byte[] data)
            => _usbProvider?.Read(data);
        public void Write(byte[] data)
            => _usbProvider?.Write(data);

        public virtual void Dispose()
        {
            _usbProvider?.Dispose();
        }
    }
}
    - cron: ﻿// Source thanks to https://github.com/vddCore/Starlight :)

namespace GHelper.AnimeMatrix.Communication
{
    public abstract class Packet
    {
        private int _currentDataIndex = 1;

        public byte[] Data { get; }

        internal Packet(byte reportId, int packetLength, params byte[] data)
        {
            if (packetLength < 1)
            {
                throw new ArgumentOutOfRangeException(
                    nameof(packetLength),
                    "Packet length must be at least 1."
                );
            }

            Data = new byte[packetLength];
            Data[0] = reportId;

            if (data.Length > 0)
            {
                if (_currentDataIndex >= Data.Length)
                {
                    throw new ArgumentOutOfRangeException(
                        nameof(data),
                        "Your packet length does not allow for initial data to be appended."
                    );
                }

                AppendData(data);
            }
        }

        public Packet AppendData(params byte[] data)
            => AppendData(out _, data);

        public Packet AppendData(out int bytesWritten, params byte[] data)
        {
            bytesWritten = 0;

            for (var i = 0;
                 i < data.Length && _currentDataIndex < Data.Length - 1;
                 i++, bytesWritten++, _currentDataIndex++)
            {
                if (_currentDataIndex > Data.Length - 1)
                    break;

                Data[_currentDataIndex] = data[i];
            }

            return this;
        }
    }
}

env: ﻿using NAudio.CoreAudioApi;
using NAudio.Wave;
using System.Diagnostics;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Timers;

namespace GHelper.AnimeMatrix
{

    public class AniMatrixControl : NAudio.CoreAudioApi.Interfaces.IMMNotificationClient
    {

        SettingsForm settings;

        System.Timers.Timer matrixTimer = default!;
        System.Timers.Timer slashTimer = default!;

        public AnimeMatrixDevice? deviceMatrix;
        public SlashDevice? deviceSlash;

        public static bool lidClose = false;
        private static bool _wakeUp = false;

        double[]? AudioValues;
        WasapiCapture? AudioDevice;
        string? AudioDeviceId;
        private MMDeviceEnumerator? AudioDeviceEnum;

        public bool IsValid => deviceMatrix != null || deviceSlash != null;
        public bool IsSlash => deviceSlash != null;

        private long lastPresent;
        private List<double> maxes = new List<double>();

        public AniMatrixControl(SettingsForm settingsForm)
        {
            settings = settingsForm;

            try
            {
                if (AppConfig.IsSlash())
                    deviceSlash = new SlashDevice();
                else
                    deviceMatrix = new AnimeMatrixDevice();

                matrixTimer = new System.Timers.Timer(100);
                matrixTimer.Elapsed += MatrixTimer_Elapsed;

            }
            catch (Exception ex)
            {
                Logger.WriteLine(ex.Message);
            }

        }

        public void SetDevice(bool wakeUp = false)
        {
            if (deviceMatrix is not null) SetMatrix(wakeUp);
            if (deviceSlash is not null) SetSlash(wakeUp);
        }

        public void SetSlash(bool wakeUp = false)
        {
            if (deviceSlash is null) return;

            int brightness = AppConfig.Get("matrix_brightness", 0);
            int running = AppConfig.Get("matrix_running", 0);
            int inteval = AppConfig.Get("matrix_interval", 0);

            bool auto = AppConfig.Is("matrix_auto");
            bool lid = AppConfig.Is("matrix_lid");

            Task.Run(() =>
            {
                try
                {
                    deviceSlash.SetProvider();
                }
                catch (Exception ex)
                {
                    Logger.WriteLine(ex.Message);
                    return;
                }

                if (wakeUp) _wakeUp = true;

                if (brightness == 0 || (auto && SystemInformation.PowerStatus.PowerLineStatus != PowerLineStatus.Online) || (lid && lidClose))
                {
                    deviceSlash.SetEnabled(false);
                    //deviceSlash.Init();
                    //deviceSlash.SetOptions(false, 0, 0);
                    deviceSlash.SetSleepActive(false);
                }
                else
                {
                    if (_wakeUp)
                    {
                        deviceSlash.WakeUp();
                        _wakeUp = false;
                    }

                    deviceSlash.SetEnabled(true);
                    deviceSlash.Init();
                    
                    switch ((SlashMode)running)
                    {
                        case SlashMode.Static:
                            var custom = AppConfig.GetString("slash_custom");
                            if (custom is not null && custom.Length > 0)
                            {
                                deviceSlash.SetCustom(AppConfig.StringToBytes(custom));
                            } else
                            {
                                deviceSlash.SetStatic(brightness);
                            }
                            break;
                        case SlashMode.BatteryLevel:
                            // call tick to immediately update the pattern
                            SlashTimer_start();
                            SlashTimer_tick();
                            break;
                        default:
                            deviceSlash.SetMode((SlashMode)running);
                            deviceSlash.SetOptions(true, brightness, inteval);
                            deviceSlash.Save();
                            break;
                    }
                    // kill the timer if we are not displaying battery pattern

                    deviceSlash.SetSleepActive(true);
                }
            });
        }

        public void SetLidMode(bool force = false)
        {
            bool matrixLid = AppConfig.Is("matrix_lid");

            if (deviceSlash is not null)
            {
                deviceSlash.SetLidMode(true);
            }

            if (matrixLid || force)
            {
                Logger.WriteLine($"Matrix LidClosed: {lidClose}");
                SetDevice(true);
            }
        }

        public void SetBatteryAuto()
        {
            if (deviceSlash is not null)
            {
                bool auto = AppConfig.Is("matrix_auto");
                deviceSlash.SetBatterySaver(auto);
                if (!auto) SetSlash();
            }

            if (deviceMatrix is not null) SetMatrix();
        }

        public void SetMatrix(bool wakeUp = false)
        {

            if (deviceMatrix is null) return;

            int brightness = AppConfig.Get("matrix_brightness", 0);
            int running = AppConfig.Get("matrix_running", 0);
            bool auto = AppConfig.Is("matrix_auto");
            bool lid = AppConfig.Is("matrix_lid");

            StopMatrixTimer();
            StopMatrixAudio();

            Task.Run(() =>
            {
                try
                {
                    deviceMatrix.SetProvider();
                }
                catch (Exception ex)
                {
                    Logger.WriteLine(ex.Message);
                    return;
                }

                if (wakeUp) deviceMatrix.WakeUp();

                if (brightness == 0 || (auto && SystemInformation.PowerStatus.PowerLineStatus != PowerLineStatus.Online) || (lid && lidClose))
                {
                    deviceMatrix.SetDisplayState(false);
                    deviceMatrix.SetDisplayState(false); // some devices are dumb
                    Logger.WriteLine("Matrix Off");
                }
                else
                {
                    deviceMatrix.SetDisplayState(true);
                    deviceMatrix.SetBrightness((BrightnessMode)brightness);

                    switch (running)
                    {
                        case 2:
                            SetMatrixPicture(AppConfig.GetString("matrix_picture"));
                            break;
                        case 3:
                            SetMatrixClock();
                            break;
                        case 4:
                            SetMatrixAudio();
                            break;
                        default:
                            SetBuiltIn(running);
                            break;
                    }

                }
            });


        }

        private void SetBuiltIn(int running)
        {
            BuiltInAnimation animation = new BuiltInAnimation(
                (BuiltInAnimation.Running)running,
                (BuiltInAnimation.Sleeping)AppConfig.Get("matrix_sleep", (int)BuiltInAnimation.Sleeping.Starfield),
                (BuiltInAnimation.Shutdown)AppConfig.Get("matrix_shutdown", (int)BuiltInAnimation.Shutdown.SeeYa),
                (BuiltInAnimation.Startup)AppConfig.Get("matrix_startup", (int)BuiltInAnimation.Startup.StaticEmergence)
            );
            deviceMatrix.SetBuiltInAnimation(true, animation);
            Logger.WriteLine("Matrix builtin: " + animation.AsByte);
        }

        private void StartMatrixTimer(int interval = 100)
        {
            matrixTimer.Interval = interval;
            matrixTimer.Start();
        }

        private void StopMatrixTimer()
        {
            matrixTimer.Stop();
        }

        private void MatrixTimer_Elapsed(object? sender, ElapsedEventArgs e)
        {

            if (deviceMatrix is null) return;

            switch (AppConfig.Get("matrix_running"))
            {
                case 2:
                    deviceMatrix.PresentNextFrame();
                    break;
                case 3:
                    deviceMatrix.PresentClock();
                    break;
            }

        }

        public void SetMatrixClock()
        {
            deviceMatrix.SetBuiltInAnimation(false);
            StartMatrixTimer(1000);
            Logger.WriteLine("Matrix Clock");
        }


        private void SlashTimer_start(int interval = 60000)
        {
            // 100% to 0% in 1hr = 1% every 36 seconds
            // 1 bracket every 14.2857 * 36s = 514s ~ 8m 30s
            // only ~5 actually distinguishable levels, so refresh every <= 514/5 ~ 100s
            // default is 60s

            // create the timer if first call
            // this way, the timer only spawns if user tries to use battery pattern
            if(slashTimer == default(System.Timers.Timer))
            {
                slashTimer = new System.Timers.Timer(interval);
                slashTimer.Elapsed += SlashTimer_elapsed;
                slashTimer.AutoReset = true;
            }
            // only write if interval changed
            if(slashTimer.Interval != interval)
            {
                slashTimer.Interval = interval;
            }

            slashTimer.Start();
        }

        private void SlashTimer_elapsed(object? sender, ElapsedEventArgs e)
        {
            SlashTimer_tick();
        }

        private void SlashTimer_tick()
        {
            if (deviceSlash is null) return;

            //kill timer if called but not in battery pattern mode
            if((SlashMode)AppConfig.Get("matrix_running", 0) != SlashMode.BatteryLevel)
            {
                slashTimer.Stop();
                slashTimer.Dispose();
                return;
            }

            deviceSlash.SetBatteryPattern(AppConfig.Get("matrix_brightness", 0));
        }


        public void Dispose()
        {
            StopMatrixAudio();
        }

        void StopMatrixAudio()
        {
            if (AudioDevice is not null)
            {
                try
                {
                    AudioDevice.StopRecording();
                    AudioDevice.Dispose();
                }
                catch (Exception ex)
                {
                    Logger.WriteLine(ex.ToString());
                }
            }

            AudioDeviceId = null;
            AudioDeviceEnum?.Dispose();
        }

        void SetMatrixAudio()
        {
            if (deviceMatrix is null) return;

            deviceMatrix.SetBuiltInAnimation(false);
            StopMatrixTimer();
            StopMatrixAudio();

            try
            {
                AudioDeviceEnum = new MMDeviceEnumerator();
                AudioDeviceEnum.RegisterEndpointNotificationCallback(this);

                using (MMDevice device = AudioDeviceEnum.GetDefaultAudioEndpoint(DataFlow.Render, Role.Console))
                {
                    AudioDevice = new WasapiLoopbackCapture(device);
                    AudioDeviceId = device.ID;
                    WaveFormat fmt = AudioDevice.WaveFormat;

                    AudioValues = new double[fmt.SampleRate / 1000];
                    AudioDevice.DataAvailable += WaveIn_DataAvailable;
                    AudioDevice.StartRecording();
                    Logger.WriteLine("Matrix Audio");
                }
            }
            catch (Exception ex)
            {
                Logger.WriteLine(ex.ToString());
            }

        }

        private void WaveIn_DataAvailable(object? sender, WaveInEventArgs e)
        {
            int bytesPerSamplePerChannel = AudioDevice.WaveFormat.BitsPerSample / 8;
            int bytesPerSample = bytesPerSamplePerChannel * AudioDevice.WaveFormat.Channels;
            int bufferSampleCount = e.Buffer.Length / bytesPerSample;

            if (bufferSampleCount >= AudioValues.Length)
            {
                bufferSampleCount = AudioValues.Length;
            }

            if (bytesPerSamplePerChannel == 2 && AudioDevice.WaveFormat.Encoding == WaveFormatEncoding.Pcm)
            {
                for (int i = 0; i < bufferSampleCount; i++)
                    AudioValues[i] = BitConverter.ToInt16(e.Buffer, i * bytesPerSample);
            }
            else if (bytesPerSamplePerChannel == 4 && AudioDevice.WaveFormat.Encoding == WaveFormatEncoding.Pcm)
            {
                for (int i = 0; i < bufferSampleCount; i++)
                    AudioValues[i] = BitConverter.ToInt32(e.Buffer, i * bytesPerSample);
            }
            else if (bytesPerSamplePerChannel == 4 && AudioDevice.WaveFormat.Encoding == WaveFormatEncoding.IeeeFloat)
            {
                for (int i = 0; i < bufferSampleCount; i++)
                    AudioValues[i] = BitConverter.ToSingle(e.Buffer, i * bytesPerSample);
            }

            double[] paddedAudio = FftSharp.Pad.ZeroPad(AudioValues);
            double[] fftMag = FftSharp.Transform.FFTmagnitude(paddedAudio);

            PresentAudio(fftMag);
        }

        private void DrawBar(int pos, double h)
        {
            int dx = pos * 2;
            int dy = 20;

            byte color;

            for (int y = 0; y < h - (h % 2); y++)
                for (int x = 0; x < 2 - (y % 2); x++)
                {
                    //color = (byte)(Math.Min(1,(h - y - 2)*2) * 255);
                    deviceMatrix.SetLedPlanar(x + dx, dy + y, (byte)(h * 255 / 30));
                    deviceMatrix.SetLedPlanar(x + dx, dy - y, 255);
                }
        }

        void PresentAudio(double[] audio)
        {

            if (deviceMatrix is null) return;

            if (Math.Abs(DateTimeOffset.Now.ToUnixTimeMilliseconds() - lastPresent) < 70) return;
            lastPresent = DateTimeOffset.Now.ToUnixTimeMilliseconds();

            deviceMatrix.Clear();

            int size = 20;
            double[] bars = new double[size];
            double max = 2, maxAverage;

            for (int i = 0; i < size; i++)
            {
                bars[i] = Math.Sqrt(audio[i] * 10000);
                if (bars[i] > max) max = bars[i];
            }

            maxes.Add(max);
            if (maxes.Count > 20) maxes.RemoveAt(0);
            maxAverage = maxes.Average();

            for (int i = 0; i < size; i++) DrawBar(20 - i, bars[i] * 20 / maxAverage);

            deviceMatrix.Present();
        }

        public void OpenMatrixPicture()
        {
            string fileName = null;

            Thread t = new Thread(() =>
            {
                OpenFileDialog of = new OpenFileDialog();
                of.Filter = "Image Files (*.bmp;*.jpg;*.jpeg,*.png,*.gif)|*.BMP;*.JPG;*.JPEG;*.PNG;*.GIF";
                if (of.ShowDialog() == DialogResult.OK)
                {
                    fileName = of.FileName;
                }
                return;
            });

            t.SetApartmentState(ApartmentState.STA);
            t.Start();
            t.Join();

            if (fileName is not null)
            {
                AppConfig.Set("matrix_picture", fileName);
                AppConfig.Set("matrix_running", 2);

                SetMatrixPicture(fileName);
                settings.VisualiseMatrixRunning(2);

            }

        }

        public void SetMatrixPicture(string fileName, bool visualise = true)
        {

            if (deviceMatrix is null) return;

            StopMatrixTimer();

            try
            {
                using (var fs = new FileStream(fileName, FileMode.Open))
                //using (var ms = new MemoryStream())
                {
                    /*
                    ms.SetLength(0);
                    fs.CopyTo(ms);
                    ms.Position = 0;
                    */
                    using (Image image = Image.FromStream(fs))
                    {
                        ProcessPicture(image);
                        Logger.WriteLine("Matrix " + fileName);
                    }

                    fs.Close();
                    if (visualise) settings.VisualiseMatrixPicture(fileName);
                }
            }
            catch
            {
                Debug.WriteLine("Error loading picture");
                return;
            }

        }

        protected void ProcessPicture(Image image)
        {
            deviceMatrix.SetBuiltInAnimation(false);
            deviceMatrix.ClearFrames();

            int matrixX = AppConfig.Get("matrix_x", 0);
            int matrixY = AppConfig.Get("matrix_y", 0);

            int matrixZoom = AppConfig.Get("matrix_zoom", 100);
            int matrixContrast = AppConfig.Get("matrix_contrast", 100);
            int matrixGamma = AppConfig.Get("matrix_gamma", 0);

            int matrixSpeed = AppConfig.Get("matrix_speed", 50);

            MatrixRotation rotation = (MatrixRotation)AppConfig.Get("matrix_rotation", 0);

            InterpolationMode matrixQuality = (InterpolationMode)AppConfig.Get("matrix_quality", 0);


            FrameDimension dimension = new FrameDimension(image.FrameDimensionsList[0]);
            int frameCount = image.GetFrameCount(dimension);

            if (frameCount > 1)
            {
                var delayPropertyBytes = image.GetPropertyItem(0x5100).Value;
                var frameDelay = BitConverter.ToInt32(delayPropertyBytes) * 10;

                for (int i = 0; i < frameCount; i++)
                {
                    image.SelectActiveFrame(dimension, i);

                    if (rotation == MatrixRotation.Planar)
                        deviceMatrix.GenerateFrame(image, matrixZoom, matrixX, matrixY, matrixQuality, matrixContrast, matrixGamma);
                    else
                        deviceMatrix.GenerateFrameDiagonal(image, matrixZoom, matrixX, matrixY, matrixQuality, matrixContrast, matrixGamma);

                    deviceMatrix.AddFrame();
                }


                Logger.WriteLine("GIF Delay:" + frameDelay);
                StartMatrixTimer(Math.Max(matrixSpeed, frameDelay));

                //image.SelectActiveFrame(dimension, 0);

            }
            else
            {
                if (rotation == MatrixRotation.Planar)
                    deviceMatrix.GenerateFrame(image, matrixZoom, matrixX, matrixY, matrixQuality, matrixContrast, matrixGamma);
                else
                    deviceMatrix.GenerateFrameDiagonal(image, matrixZoom, matrixX, matrixY, matrixQuality, matrixContrast, matrixGamma);

                deviceMatrix.Present();
            }

        }

        public void OnDeviceStateChanged(string deviceId, DeviceState newState)
        {

        }

        public void OnDeviceAdded(string pwstrDeviceId)
        {

        }

        public void OnDeviceRemoved(string deviceId)
        {

        }

        public void OnDefaultDeviceChanged(DataFlow flow, Role role, string defaultDeviceId)
        {
            if (AudioDeviceId == defaultDeviceId)
            {
                //We already caputre this device. No need to re-initialize
                return;
            }

            int running = AppConfig.Get("matrix_running");
            if (flow != DataFlow.Render || role != Role.Console || running != 4)
            {
                return;
            }

            //Restart audio if default audio changed
            Logger.WriteLine("Matrix Audio: Default Output changed to " + defaultDeviceId);

            //Already set the device here. Otherwise this will be called multiple times in a short succession and causes a crash due to dispose during initalization.
            AudioDeviceId = defaultDeviceId;

            //Delay is required or it will deadlock on dispose.
            Task.Delay(50).ContinueWith(t => SetMatrixAudio());
        }

        public void OnPropertyValueChanged(string pwstrDeviceId, PropertyKey key)
        {

        }
    }
}
  # Path to the CMake build directory.
  build: '${{ ﻿// Source thanks to https://github.com/vddCore/Starlight with some adjustments from me

using GHelper.AnimeMatrix.Communication;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Text;

namespace GHelper.AnimeMatrix
{
    public class BuiltInAnimation
    {
        public enum Startup
        {
            GlitchConstruction,
            StaticEmergence
        }

        public enum Shutdown
        {
            GlitchOut,
            SeeYa
        }

        public enum Sleeping
        {
            BannerSwipe,
            Starfield
        }

        public enum Running
        {
            BinaryBannerScroll,
            RogLogoGlitch
        }

        public byte AsByte { get; }

        public BuiltInAnimation(
            Running running,
            Sleeping sleeping,
            Shutdown shutdown,
            Startup startup
        )
        {
            AsByte |= (byte)(((int)running & 0x01) << 0);
            AsByte |= (byte)(((int)sleeping & 0x01) << 1);
            AsByte |= (byte)(((int)shutdown & 0x01) << 2);
            AsByte |= (byte)(((int)startup & 0x01) << 3);
        }
    }

    public enum MatrixRotation
    {
        Planar,
        Diagonal
    }

    internal class AnimeMatrixPacket : Packet
    {
        public AnimeMatrixPacket(byte[] command)
            : base(0x5E, 640, command)
        {
        }
    }

    public enum AnimeType
    {
        GA401,
        GA402,
        GU604
    }


    public enum BrightnessMode : byte
    {
        Off = 0,
        Dim = 1,
        Medium = 2,
        Full = 3
    }


    public class AnimeMatrixDevice : Device
    {
        int UpdatePageLength = 490;
        int LedCount = 1450;

        byte[] _displayBuffer;
        List<byte[]> frames = new List<byte[]>();

        public int MaxRows = 61;
        public int MaxColumns = 34;
        public int LedStart = 0;
        public int FullRows = 11;

        private int frameIndex = 0;

        private static AnimeType _model = AnimeType.GA402;

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        private static extern IntPtr AddFontMemResourceEx(IntPtr pbFont, uint cbFont, IntPtr pdv, [System.Runtime.InteropServices.In] ref uint pcFonts);
        private PrivateFontCollection fonts = new PrivateFontCollection();

        public AnimeMatrixDevice() : base(0x0B05, 0x193B, 640)
        {
            if (AppConfig.ContainsModel("401"))
            {
                _model = AnimeType.GA401;

                MaxColumns = 33;
                MaxRows = 55;
                LedCount = 1245;

                UpdatePageLength = 410;

                FullRows = 5;

                LedStart = 1;
            }

            if (AppConfig.ContainsModel("GU604"))
            {
                _model = AnimeType.GU604;

                MaxColumns = 39;
                MaxRows = 92;
                LedCount = 1711;
                UpdatePageLength = 630;

                FullRows = 9;
            }

            _displayBuffer = new byte[LedCount];

            LoadMFont();

        }

        public void WakeUp()
        {
            Set(Packet<AnimeMatrixPacket>(Encoding.ASCII.GetBytes("ASUS Tech.Inc.")));
        }

        public void SetBrightness(BrightnessMode mode)
        {
            Set(Packet<AnimeMatrixPacket>(0xC0, 0x04, (byte)mode));
        }

        public void SetDisplayState(bool enable)
        {
            Set(Packet<AnimeMatrixPacket>(0xC3, 0x01, enable ? (byte)0x00 : (byte)0x80));
        }

        public void SetBuiltInAnimation(bool enable)
        {
            Set(Packet<AnimeMatrixPacket>(0xC4, 0x01, enable ? (byte)0x00 : (byte)0x80));
        }

        public void SetBuiltInAnimation(bool enable, BuiltInAnimation animation)
        {
            SetBuiltInAnimation(enable);
            Set(Packet<AnimeMatrixPacket>(0xC5, animation.AsByte));
        }

        public void Present()
        {

            int page = 0;
            int start, end;

            while (page * UpdatePageLength < LedCount)
            {
                start = page * UpdatePageLength;
                end = Math.Min(LedCount, (page + 1) * UpdatePageLength);

                Set(Packet<AnimeMatrixPacket>(0xC0, 0x02)
                    .AppendData(BitConverter.GetBytes((ushort)(start + 1)))
                    .AppendData(BitConverter.GetBytes((ushort)(end - start)))
                    .AppendData(_displayBuffer[start..end])
                );

                page++;
            }

            Set(Packet<AnimeMatrixPacket>(0xC0, 0x03));
        }


        private void LoadMFont()
        {
            byte[] fontData = GHelper.Properties.Resources.MFont;
            IntPtr fontPtr = System.Runtime.InteropServices.Marshal.AllocCoTaskMem(fontData.Length);
            System.Runtime.InteropServices.Marshal.Copy(fontData, 0, fontPtr, fontData.Length);
            uint dummy = 0;

            fonts.AddMemoryFont(fontPtr, GHelper.Properties.Resources.MFont.Length);
            AddFontMemResourceEx(fontPtr, (uint)GHelper.Properties.Resources.MFont.Length, IntPtr.Zero, ref dummy);
            System.Runtime.InteropServices.Marshal.FreeCoTaskMem(fontPtr);
        }


        public void PresentNextFrame()
        {
            if (frameIndex >= frames.Count) frameIndex = 0;
            _displayBuffer = frames[frameIndex];
            Present();
            frameIndex++;
        }

        public void ClearFrames()
        {
            frames.Clear();
            frameIndex = 0;
        }

        public void AddFrame()
        {
            frames.Add(_displayBuffer.ToArray());
        }

        public int Width()
        {
            switch (_model)
            {
                case AnimeType.GA401:
                    return 33;
                case AnimeType.GU604:
                    return 39;
                default:
                    return 34;
            }
        }

        public int FirstX(int y)
        {
            switch (_model)
            {
                case AnimeType.GA401:
                    if (y < 5 && y % 2 == 0)
                    {
                        return 1;
                    }
                    return (int)Math.Ceiling(Math.Max(0, y - 5) / 2F);
                case AnimeType.GU604:
                    if (y < 9 && y % 2 == 0)
                    {
                        return 1;
                    }
                    return (int)Math.Ceiling(Math.Max(0, y - 9) / 2F);

                default:
                    return (int)Math.Ceiling(Math.Max(0, y - 11) / 2F);
            }
        }


        public int Pitch(int y)
        {
            switch (_model)
            {
                case AnimeType.GA401:
                    switch (y)
                    {
                        case 0:
                        case 2:
                        case 4:
                            return 33;
                        case 1:
                        case 3:
                            return 35;
                        default:
                            return 36 - y / 2;
                    }

                case AnimeType.GU604:
                    switch (y)
                    {
                        case 0:
                        case 2:
                        case 4:
                        case 6:
                        case 8:
                            return 38;

                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 9:
                            return 39;

                        default:
                            return Width() - FirstX(y);
                    }


                default:
                    return Width() - FirstX(y);
            }
        }


        public int RowToLinearAddress(int y)
        {
            int ret = LedStart;
            for (var i = 0; i < y; i++)
                ret += Pitch(i);

            return ret;
        }

        public void SetLedPlanar(int x, int y, byte value)
        {
            if (!IsRowInRange(y)) return;

            if (x >= FirstX(y) && x < Width())
                SetLedLinear(RowToLinearAddress(y) - FirstX(y) + x, value);
            }

        public void SetLedDiagonal(int x, int y, byte color, int deltaX = 0, int deltaY = 0)
        {
            x += deltaX;
            y -= deltaY;

            int plX = (x - y) / 2;
            int plY = x + y;

            if (x - y == -1) plX = -1;

            SetLedPlanar(plX, plY, color);
        }


        public void SetLedLinear(int address, byte value)
        {
            if (!IsAddressableLed(address)) return;
            _displayBuffer[address] = value;
        }


        public void Clear(bool present = false)
        {
            for (var i = 0; i < _displayBuffer.Length; i++) _displayBuffer[i] = 0;
            if (present) Present();
        }

        private void SetBitmapDiagonal(Bitmap bmp, int deltaX = 0, int deltaY = 0, int contrast = 100, int gamma = 0)
        {
            for (int y = 0; y < bmp.Height; y++)
            {
                for (int x = 0; x < bmp.Width; x++)
                {
                    var pixel = bmp.GetPixel(x, y);
                    var color = Math.Min((pixel.R + pixel.G + pixel.B + gamma) * contrast / 300, 255);
                    if (color > 20)
                        SetLedDiagonal(x, y, (byte)color, deltaX, deltaY - (FullRows / 2) - 1);
                }
            }
        }

        private void SetBitmapLinear(Bitmap bmp, int contrast = 100, int gamma = 0)
        {
            for (int y = 0; y < bmp.Height; y++)
            {
                for (int x = 0; x < bmp.Width; x++)
                    if (x % 2 == y % 2)
                    {
                        var pixel = bmp.GetPixel(x, y);
                        var color = Math.Min((pixel.R + pixel.G + pixel.B + gamma) * contrast / 300, 255);
                        if (color > 20)
                            SetLedPlanar(x / 2, y, (byte)color);
                    }
            }
        }

        public void Text(string text, float fontSize = 10, int x = 0, int y = 0)
        {

            int width = MaxRows;
            int height = MaxRows - FullRows;
            int textHeight, textWidth;

            using (Bitmap bmp = new Bitmap(width, height))
            {
                using (Graphics g = Graphics.FromImage(bmp))
                {
                    g.CompositingQuality = CompositingQuality.HighQuality;
                    g.SmoothingMode = SmoothingMode.AntiAlias;
                    g.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;

                    using (Font font = new Font(fonts.Families[0], fontSize, FontStyle.Regular, GraphicsUnit.Pixel))
                    {
                        SizeF textSize = g.MeasureString(text, font);
                        textHeight = (int)textSize.Height;
                        textWidth = (int)textSize.Width;
                        g.DrawString(text, font, Brushes.White, x, height - y);
                    }
                }

                SetBitmapDiagonal(bmp, 5, height);

            }
        }

        public void PresentClock()
        {
            string second = (DateTime.Now.Second % 2 == 0) ? ":" : "  ";
            string time = DateTime.Now.ToString("HH" + second + "mm");

            Clear();
            Text(time, 15, 0, 25);
            Text(DateTime.Now.ToString("yy'. 'MM'. 'dd"), 11.5F, 0, 14);
            Present();

        }
        public void GenerateFrame(Image image, float zoom = 100, int panX = 0, int panY = 0, InterpolationMode quality = InterpolationMode.Default, int contrast = 100, int gamma = 0)
        {
            int width = MaxColumns / 2 * 6;
            int height = MaxRows;

            int targetWidth = MaxColumns * 2;

            float scale;

            using (Bitmap bmp = new Bitmap(targetWidth, height))
            {
                scale = Math.Min((float)width / (float)image.Width, (float)height / (float)image.Height) * zoom / 100;

                using (var graph = Graphics.FromImage(bmp))
                {
                    var scaleWidth = (float)(image.Width * scale);
                    var scaleHeight = (float)(image.Height * scale);

                    graph.InterpolationMode = quality;
                    graph.CompositingQuality = CompositingQuality.HighQuality;
                    graph.SmoothingMode = SmoothingMode.AntiAlias;

                    graph.DrawImage(image, (float)Math.Round(targetWidth - (scaleWidth + panX) * targetWidth / width), -panY, (float)Math.Round(scaleWidth * targetWidth / width), scaleHeight);

                }

                Clear();
                SetBitmapLinear(bmp, contrast, gamma);
            }
        }

        public void GenerateFrameDiagonal(Image image, float zoom = 100, int panX = 0, int panY = 0, InterpolationMode quality = InterpolationMode.Default, int contrast = 100, int gamma = 0)
        {
            int width = MaxRows + FullRows;
            int height = MaxColumns + FullRows;

            if ((image.Height / image.Width) > (height / width)) height = MaxColumns;

            float scale;

            using (Bitmap bmp = new Bitmap(width, height))
            {
                scale = Math.Min((float)width / (float)image.Width, (float)height / (float)image.Height) * zoom / 100;

                using (var graph = Graphics.FromImage(bmp))
                {
                    var scaleWidth = (float)(image.Width * scale);
                    var scaleHeight = (float)(image.Height * scale);

                    graph.InterpolationMode = quality;
                    graph.CompositingQuality = CompositingQuality.HighQuality;
                    graph.SmoothingMode = SmoothingMode.AntiAlias;

                    graph.DrawImage(image, (width - scaleWidth) / 2, height - scaleHeight, scaleWidth, scaleHeight);

                }

                Clear();
                SetBitmapDiagonal(bmp, -panX, height + panY, contrast, gamma);
            }
        }


        private bool IsRowInRange(int row)
        {
            return (row >= 0 && row < MaxRows);
        }

        private bool IsAddressableLed(int address)
        {
            return (address >= 0 && address < LedCount);
        }
    }
} }}/build'

permissions: ﻿using GHelper.AnimeMatrix.Communication;
using System.Management;
using System.Text;
using System.Timers;

namespace GHelper.AnimeMatrix
{
    public enum SlashMode
    {
        Bounce,
        Slash,
        Loading,
        BitStream,
        Transmission,
        Flow,
        Flux,
        Phantom,
        Spectrum,
        Hazard,
        Interfacing,
        Ramp,
        GameOver,
        Start,
        Buzzer,
        Static,
        BatteryLevel,
    }

    internal class SlashPacket : Packet
    {
        public SlashPacket(byte[] command) : base(0x5E, 128, command)
        {
        }
    }

    public class SlashDevice : Device
    {

        public static Dictionary<SlashMode, string> Modes = new Dictionary<SlashMode, string>
        {
            { SlashMode.Bounce, "Bounce"},
            { SlashMode.Slash, "Slash"},
            { SlashMode.Loading, "Loading"},

            { SlashMode.BitStream, "Bit Stream"},
            { SlashMode.Transmission, "Transmission"},

            { SlashMode.Flow, "Flow"},
            { SlashMode.Flux, "Flux"},
            { SlashMode.Phantom, "Phantom"},
            { SlashMode.Spectrum, "Spectrum"},

            { SlashMode.Hazard, "Hazard"},
            { SlashMode.Interfacing, "Interfacing"},
            { SlashMode.Ramp, "Ramp"},

            { SlashMode.GameOver, "Game Over"},
            { SlashMode.Start, "Start"},
            { SlashMode.Buzzer, "Buzzer"},

            { SlashMode.Static, "Static"},
            { SlashMode.BatteryLevel, "Battery Level"}
        };

        private static Dictionary<SlashMode, byte> modeCodes = new Dictionary<SlashMode, byte>
        {
            { SlashMode.Bounce, 0x10},
            { SlashMode.Slash, 0x12},
            { SlashMode.Loading, 0x13},

            { SlashMode.BitStream, 0x1D},
            { SlashMode.Transmission, 0x1A},

            { SlashMode.Flow, 0x19},
            { SlashMode.Flux, 0x25},
            { SlashMode.Phantom, 0x24},
            { SlashMode.Spectrum, 0x26},

            { SlashMode.Hazard, 0x32},
            { SlashMode.Interfacing, 0x33},
            { SlashMode.Ramp, 0x34},

            { SlashMode.GameOver, 0x42},
            { SlashMode.Start, 0x43},
            { SlashMode.Buzzer, 0x44},
        };

        public SlashDevice() : base(0x0B05, 0x193B, 128)
        {
        }

        public void WakeUp()
        {
            Set(Packet<SlashPacket>(Encoding.ASCII.GetBytes("ASUS Tech.Inc.")), "SlashWakeUp");
            Set(Packet<SlashPacket>(0xC2), "SlashWakeUp");
            Set(Packet<SlashPacket>(0xD1, 0x01, 0x00, 0x01), "SlashWakeUp");
        }

        public void Init()
        {
            Set(Packet<SlashPacket>(0xD7, 0x00, 0x00, 0x01, 0xAC), "SlashInit");
            Set(Packet<SlashPacket>(0xD2, 0x02, 0x01, 0x08, 0xAB), "SlashInit");
        }

        public void SetEnabled(bool status = true)
        {
            Set(Packet<SlashPacket>(0xD8, 0x02, 0x00, 0x01, status ? (byte)0x00 : (byte)0x80), $"SlashEnable {status}");
        }

        public void Save()
        {
            Set(Packet<SlashPacket>(0xD4, 0x00, 0x00, 0x01, 0xAB), "SlashSave");
        }

        public void SetMode(SlashMode mode)
        {
            byte modeByte;

            try
            {
                modeByte = modeCodes[mode];
            }
            catch (Exception)
            {
                modeByte = 0x00;
            }

            Set(Packet<SlashPacket>(0xD2, 0x03, 0x00, 0x0C), "SlashMode");
            Set(Packet<SlashPacket>(0xD3, 0x04, 0x00, 0x0C, 0x01, modeByte, 0x02, 0x19, 0x03, 0x13, 0x04, 0x11, 0x05, 0x12, 0x06, 0x13), "SlashMode");
        }

        public void SetStatic(int brightness = 0)
        {
            SetCustom(Enumerable.Repeat((byte)(brightness * 85.333), 7).ToArray());
        }

        public static double GetBatteryChargePercentage()
            {
                double batteryCharge = 0;
                try
                {
                    ManagementObjectSearcher searcher = new ManagementObjectSearcher("SELECT * FROM Win32_Battery");
                    foreach (ManagementObject battery in searcher.Get())
                    {
                        batteryCharge = Convert.ToDouble(battery["EstimatedChargeRemaining"]);
                        break; // Assuming only one battery
                    }
                }
                catch (ManagementException e)
                {
                    Console.WriteLine("An error occurred while querying for WMI data: " + e.Message);
                }
                return batteryCharge;
            }

        private byte[] GetBatteryPattern(int brightness, double percentage)
        {
            // because 7 segments, within each led segment represents a percentage bracket of (100/7 = 14.2857%)
            // set brightness to reflect battery's percentage within that range

            int bracket = (int)Math.Floor(percentage / 14.2857);
            if(bracket >= 7) return Enumerable.Repeat((byte)(brightness * 85.333), 7).ToArray();

            byte[] batteryPattern = Enumerable.Repeat((byte)(0x00), 7).ToArray();
            for (int i = 6; i > 6-bracket; i--)
            {
                batteryPattern[i] = (byte)(brightness * 85.333);
            }

            //set the "selected" bracket to the percentage of that bracket filled from 0 to 255 as a hex
            batteryPattern[6-bracket] = (byte)(((percentage % 14.2857) * brightness * 85.333) / 14.2857);

            return batteryPattern;
        }

        public void SetBatteryPattern(int brightness)
        {
            SetCustom(GetBatteryPattern(brightness, 100*(GetBatteryChargePercentage()/AppConfig.Get("charge_limit",100))));
        }

        public void SetCustom(byte[] data)
        {
            Set(Packet<SlashPacket>(0xD2, 0x02, 0x01, 0x08, 0xAC), "Static");
            Set(Packet<SlashPacket>(0xD3, 0x03, 0x01, 0x08, 0xAC, 0xFF, 0xFF, 0x01, 0x05, 0xFF, 0xFF), "StaticSettings");
            Set(Packet<SlashPacket>(0xD4, 0x00, 0x00, 0x01, 0xAC), "StaticSave");

            byte[] payload = new byte[] { 0xD3, 0x00, 0x00, 0x07 };
            Set(Packet<SlashPacket>(payload.Concat(data.Take(7)).ToArray()), "Static Data");
        }

        public void SetOptions(bool status, int brightness = 0, int interval = 0)
        {
            byte brightnessByte = (byte)(brightness * 85.333);

            Set(Packet<SlashPacket>(0xD3, 0x03, 0x01, 0x08, 0xAB, 0xFF, 0x01, status ? (byte)0x01 : (byte)0x00, 0x06, brightnessByte, 0xFF, (byte)interval), "SlashOptions");
        }

        public void SetBatterySaver(bool status)
        {
            Set(Packet<SlashPacket>(0xD8, 0x01, 0x00, 0x01, status ? (byte)0x80 : (byte)0x00), $"SlashBatterySaver {status}");
        }

        public void SetLidMode(bool status)
        {
            Set(Packet<SlashPacket>(0xD8, 0x00, 0x00, 0x02, 0xA5, status ? (byte)0x80 : (byte)0x00), $"DisableLidClose {status}");
        }

        public void SetSleepActive(bool status)
        {
            Set(Packet<SlashPacket>(0xD2, 0x02, 0x01, 0x08, 0xA1), "SleepInit");
            Set(Packet<SlashPacket>(0xD3, 0x03, 0x01, 0x08, 0xA1, 0x00, 0xFF, status ? (byte)0x01 : (byte)0x00, 0x02, 0xFF, 0xFF), $"Sleep {status}");
        }

        public void Set(Packet packet, string? log = null)
        {
            _usbProvider?.Set(packet.Data);
            if (log is not null) Logger.WriteLine($"{log}:" + BitConverter.ToString(packet.Data).Substring(0,48));
        }


    }
}
  contents: ﻿using GHelper.Helpers;
using System.Diagnostics;
using System.Net;
using System.Reflection;
using System.Text.Json;

namespace GHelper.AutoUpdate
{
    public class AutoUpdateControl
    {

        SettingsForm settings;

        public string versionUrl = "http://github.com/seerge/g-helper/releases";
        static long lastUpdate;

        public AutoUpdateControl(SettingsForm settingsForm)
        {
            settings = settingsForm;
            var appVersion = new Version(Assembly.GetExecutingAssembly().GetName().Version.ToString());
            settings.SetVersionLabel(Properties.Strings.VersionLabel + $": {appVersion.Major}.{appVersion.Minor}.{appVersion.Build}");
        }

        public void CheckForUpdates()
        {
            // Run update once per 12 hours
            if (Math.Abs(DateTimeOffset.Now.ToUnixTimeSeconds() - lastUpdate) < 43200) return;
            lastUpdate = DateTimeOffset.Now.ToUnixTimeSeconds();

            Task.Run(async () =>
            {
                await Task.Delay(TimeSpan.FromSeconds(1));
                CheckForUpdatesAsync();
            });
        }

        public void LoadReleases()
        {
            Process.Start(new ProcessStartInfo(versionUrl) { UseShellExecute = true });
        }

        async void CheckForUpdatesAsync()
        {

            if (AppConfig.Is("skip_updates")) return;

            try
            {

                using (var httpClient = new HttpClient())
                {
                    httpClient.DefaultRequestHeaders.Add("User-Agent", "C# App");
                    var json = await httpClient.GetStringAsync("https://api.github.com/repos/seerge/g-helper/releases/latest");
                    var config = JsonSerializer.Deserialize<JsonElement>(json);
                    var tag = config.GetProperty("tag_name").ToString().Replace("v", "");
                    var assets = config.GetProperty("assets");

                    string url = null;

                    for (int i = 0; i < assets.GetArrayLength(); i++)
                    {
                        if (assets[i].GetProperty("browser_download_url").ToString().Contains(".zip"))
                            url = assets[i].GetProperty("browser_download_url").ToString();
                    }

                    if (url is null)
                        url = assets[0].GetProperty("browser_download_url").ToString();

                    var gitVersion = new Version(tag);
                    var appVersion = new Version(Assembly.GetExecutingAssembly().GetName().Version.ToString());
                    //appVersion = new Version("0.50.0.0"); 

                    if (gitVersion.CompareTo(appVersion) > 0)
                    {
                        versionUrl = url;
                        settings.SetVersionLabel(Properties.Strings.DownloadUpdate + ": " + tag, true);

                        if (AppConfig.GetString("skip_version") != tag)
                        {
                            DialogResult dialogResult = MessageBox.Show(Properties.Strings.DownloadUpdate + ": G-Helper " + tag + "?", "Update", MessageBoxButtons.YesNo);
                            if (dialogResult == DialogResult.Yes)
                                AutoUpdate(url);
                            else
                                AppConfig.Set("skip_version", tag);
                        }

                    }
                    else
                    {
                        Logger.WriteLine($"Latest version {appVersion}");
                    }

                }
            }
            catch (Exception ex)
            {
                Logger.WriteLine("Failed to check for updates:" + ex.Message);
            }

        }


        async void AutoUpdate(string requestUri)
        {

            Uri uri = new Uri(requestUri);
            string zipName = Path.GetFileName(uri.LocalPath);

            string exeLocation = Application.ExecutablePath;
            string exeDir = Path.GetDirectoryName(exeLocation);
            string exeName = Path.GetFileName(exeLocation);
            string zipLocation = exeDir + "\\" + zipName;

            using (WebClient client = new WebClient())
            {
                client.DownloadFile(uri, zipLocation);

                Logger.WriteLine(requestUri);
                Logger.WriteLine(exeDir);
                Logger.WriteLine(zipName);
                Logger.WriteLine(exeName);

                string command = $"$ErrorActionPreference = \"Stop\"; Wait-Process -Name \"GHelper\"; Expand-Archive \"{zipName}\" -DestinationPath . -Force; Remove-Item \"{zipName}\" -Force; \".\\{exeName}\"; "; 
                Logger.WriteLine(command);

                try
                {
                    var cmd = new Process();
                    cmd.StartInfo.WorkingDirectory = exeDir;
                    cmd.StartInfo.UseShellExecute = false;
                    cmd.StartInfo.CreateNoWindow = true;
                    cmd.StartInfo.FileName = "powershell";
                    cmd.StartInfo.Arguments = command;
                    if (ProcessHelper.IsUserAdministrator()) cmd.StartInfo.Verb = "runas";
                    cmd.Start();
                }
                catch (Exception ex)
                {
                    Logger.WriteLine(ex.Message);
                }

                Environment.Exit(0);
            }

        }

    }
}

jobs: ﻿using System.Diagnostics;

namespace GHelper.Battery
{
    internal class BatteryControl
    {

        public static void ToggleBatteryLimitFull()
        {
            if (AppConfig.Is("charge_full")) SetBatteryChargeLimit();
            else SetBatteryLimitFull();
        }

        public static void SetBatteryLimitFull()
        {
            AppConfig.Set("charge_full", 1);
            Program.acpi.DeviceSet(AsusACPI.BatteryLimit, 100, "BatteryLimit");
            Program.settingsForm.VisualiseBatteryFull();
        }

        public static void UnSetBatteryLimitFull()
        {
            AppConfig.Set("charge_full", 0);
            Program.settingsForm.Invoke(Program.settingsForm.VisualiseBatteryFull);
        }

        public static void AutoBattery(bool init = false)
        {
            if (AppConfig.Is("charge_full") && !init) SetBatteryLimitFull();
            else SetBatteryChargeLimit();
        }

        public static void SetBatteryChargeLimit(int limit = -1)
        {

            if (limit < 0) limit = AppConfig.Get("charge_limit");
            if (limit < 40 || limit > 100) return;

            if (AppConfig.IsChargeLimit6080())
            {
                if (limit > 80) limit = 100;
                else if (limit < 60) limit = 60;
            }

            Program.acpi.DeviceSet(AsusACPI.BatteryLimit, limit, "BatteryLimit");

            AppConfig.Set("charge_limit", limit);
            AppConfig.Set("charge_full", 0);

            Program.settingsForm.VisualiseBattery(limit);
        }

        public static void BatteryReport()
        {
            var reportDir = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);

            try
            {
                var cmd = new Process();
                cmd.StartInfo.WorkingDirectory = reportDir;
                cmd.StartInfo.UseShellExecute = false;
                cmd.StartInfo.CreateNoWindow = true;
                cmd.StartInfo.FileName = "powershell";
                cmd.StartInfo.Arguments = "powercfg /batteryreport; explorer battery-report.html";
                cmd.Start();
            }
            catch (Exception ex)
            {
                Logger.WriteLine(ex.Message);
            }
        }

    }
}
  analyze: ﻿using GHelper.Helpers;
using System.IO.Compression;

namespace GHelper.Display
{
    public static class ColorProfileHelper
    {
        public static string? GetProfileUrl(string model)
        {
            byte[] data = Convert.FromBase64String("aHR0cHM6Ly9kbGNkbi1yb2dib3hidTEuYXN1cy5jb20vcHViL0FTVVMvQVBTZXJ2aWNlL0dhbWluZy9TWVMvUk9HUy8=");
            string baseUrl = System.Text.Encoding.UTF8.GetString(data);

            var profiles = new Dictionary<string, string>()
            {
                {"FA_Series", "2177-APOR41-de476ec68578c865e886ef6872aa56ee.zip"},
                {"FA506IC", "7822-ZGQ1KC-bc80be3be7ca585bef60f526b6461c86.zip"},
                {"FA506ICB", "13914-2HYCD7-95fabfe5669a204620517f0158c0cea1.zip"},
                {"FA506IE", "7828-57N8GH-9fe07a7e4ebde12a8d4682d3ffa66fe4.zip"},
                {"FA506IEB", "14661-ZYWH5T-3b45ebeb7fca81d98796d5960e11226c.zip"},
                {"FA506IH", "3407-VZBOUN-8a12385fa46e077010677d330e9320d5.zip"},
                {"FA506IHR", "7823-15HGA0-44d621a3c0f0dd2c407f67d262871be0.zip"},
                {"FA506II", "3410-9ZXQO2-aa08794537699c5dbfcca49b63fb7f8d.zip"},
                {"FA506IM", "7824-EKKW9A-21c7e073a81fc868b78e605b17290839.zip"},
                {"FA506IR", "7825-G6XND9-c16ea7e98b58e878c61d1071b100ee60.zip"},
                {"FA506IU", "3413-AMCR39-2a8b889a8810da4e8692f87e1ae3ea2c.zip"},
                {"FA506IV", "3415-VA0CJ6-266febe84bcecc5ed89fdc286f444ce2.zip"},
                {"FA506NC", "18359-BM2XCN-3ca9de427310b05afa92dcbe1babf947.zip"},
                {"FA506NF", "18361-CU7H6I-55beba8445b9aa0fe9cbeb8ec9a47ccf.zip"},
                {"FA506QC", "7829-LBYY5U-b14343c260b738563b69ad0e4f4b88d1.zip"},
                {"FA506QE", "7827-75P3DP-b664f4da04e77ed1faa722bf0c781d82.zip"},
                {"FA506QM", "9440-87VQKP-48357026f1f3099cfed1161a320587f0.zip"},
                {"FA506QR", "7830-KO7FQ6-618480f72c2e68bafc594b5323a5d13c.zip"},
                {"FA507NI", "14451-N6AX9R-0bc8e15be5cf889c755aaa7052f6fee8.zip"},
                {"FA507NJ", "14454-QXKHOE-d28f2eb001f9b7ce0582cbe33961a6ad.zip"},
                {"FA507NU", "14452-EZXLVF-c64dd3d7efede78df653b029888d9d04.zip"},
                {"FA507NUR", "18871-WR3YMP-6cbcd0502eb9bf200465e36c4de8c9fd.zip"},
                {"FA507NV", "14453-VC333P-41549f64788d59c9711eccec892fa8c1.zip"},
                {"FA507NVR", "18872-HVR4UY-220dc7f3f73cf12b2af2505db6ce67ab.zip"},
                {"FA507RC", "11448-0TRT8V-7712eb9302300dd82815027efbbe4787.zip"},
                {"FA507RE", "11449-ONC7VG-a7ac70468292336f63532c21204d69d4.zip"},
                {"FA507RF", "14311-X72FIU-66bf16069a82bdf2cc09f8b92cb25c67.zip"},
                {"FA507RH", "11453-W4B8H4-e1645d3ce1e0216a30246ad6a84c3819.zip"},
                {"FA507RM", "11452-ZGHOOO-f3bddf2fddd3c027568646bc5afe3c7d.zip"},
                {"FA507RR", "11450-UU8KFV-2c718d16687fa3b038e625f504ccd13f.zip"},
                {"FA507RW", "11451-XHL0SH-c2702d2047b7b93636cc1372caf25a75.zip"},
                {"FA507UI", "17850-4J1GCV-c30e3d47aa82df4e3f0754888903c5dd.zip"},
                {"FA507UU", "17851-PIGBD6-45227ab485302ec2cca63f0bbb71d473.zip"},
                {"FA507UV", "17852-T32MPT-a2fd989758365b8a04e4dc5bbb146af1.zip"},
                {"FA507XI", "14817-A522MM-fad14cb28f00b94735eb55c2b3b7b892.zip"},
                {"FA507XJ", "14820-8RT0TF-9d341fae2a290494206deb5fcf1fd427.zip"},
                {"FA507XU", "14818-3PA2GD-2d1e2d035309ec7282a1c47e27f7c7fd.zip"},
                {"FA507XV", "14819-ET4L73-daf905274776ae9c92a7b74e687dc63d.zip"},
                {"FA607PI", "17579-FL63B4-df08bf92fe3c156ce738e43de7824d65.zip"},
                {"FA607PU", "17580-WM413K-eff7721ca9ec5b633ef196dbfcd58ff2.zip"},
                {"FA607PV", "17581-JNXO2Q-15d026a4ae630d44b28c18aee0a99e51.zip"},
                {"FA607RC", "10190-XHDRTQ-0e6c248e3dab6bb07e052edd963c5218.zip"},
                {"FA607RE", "10194-AF495K-23a4d74be5132e4babc64bb13237a12b.zip"},
                {"FA607RH", "10191-9BS333-c6deb5f9fd5a37d446d2722da54c0ec3.zip"},
                {"FA607RM", "10195-4U5EEF-cf98d168cacdd5a60782e881a9c143be.zip"},
                {"FA607RR", "10193-FYX2N7-730c76e49ef71a6164804eafd2621dab.zip"},
                {"FA607RW", "10192-UFOOP3-f52a50dac189ffa96560f73e04c53a6c.zip"},
                {"FA617NS", "15967-YKHJ0B-89095e43578e64c160d2952bb14513f2.zip"},
                {"FA617NT", "15970-C1BW39-2ef1c5d5d9a967cf2fc1f511d3db3a43.zip"},
                {"FA617XS", "15968-Y9NFXN-cb420fe6e177fc1cf675e9d42a6fd0ea.zip"},
                {"FA617XT", "15969-UKYP5L-ad54051ebbad4b17693b65f5bceda799.zip"},
                {"FA706IC", "5215-5J3H5Y-d2cdc2f5e03d68e5e5e562581a77c8e6.zip"},
                {"FA706IE", "8827-43DTPG-ba6be621f7d0dda88e5391164cbc7a74.zip"},
                {"FA706IH", "2610-2EJ0EQ-245d43d7bd14bb3e664fa96e9f5a690e.zip"},
                {"FA706IHR", "8828-XBJAQU-5d8064b0fb63ca7866ee4cf821ece676.zip"},
                {"FA706II", "2611-HO3IK9-2fb972a395137c940151723b01e9b9cb.zip"},
                {"FA706IM", "8829-GWFKN5-5ab773eb09bfb3099137fae3c7d547c1.zip"},
                {"FA706IR", "8825-63J2NY-2496ecf21b5fa71e65671531b670cdac.zip"},
                {"FA706IU", "2608-EI64MF-64c35bda7789e6e71298e22d793b78d4.zip"},
                {"FA706NF", "18362-1V7Z1Z-0bc79f01bdfadc5b39342fba622a5b8d.zip"},
                {"FA706QC", "8826-032S4E-868933565b1a7d762bda8cafc1e77458.zip"},
                {"FA706QE", "8824-KZNJL9-9d827f4973d4c9787ff050dc1bf35eef.zip"},
                {"FA706QM", "8830-6NX7U4-b7bbfe663aa954534d33d2f822181728.zip"},
                {"FA706QR", "8823-RXSLAC-a2720365e842af1dd826920fb447fba5.zip"},
                {"FA707NI", "14455-19J6WZ-10282c6083ee018a614e75bc809ecf1f.zip"},
                {"FA707NJ", "14457-BGJD0K-d963ff94009c9a2a5b99606eac711a95.zip"},
                {"FA707NU", "14456-VAZ4LS-1d798d3873c55e9d10b43a3f2b12cd15.zip"},
                {"FA707NV", "14458-7G8B40-08117ddb45ed3c6978aa4ad60e3d2c96.zip"},
                {"FA707RC", "10196-81D3O0-94ccf4d908c9bcb44f8f9092ef9fa2e2.zip"},
                {"FA707RE", "10200-AUS9AW-7f366055be2ae2b4c659aa9590bb4d5e.zip"},
                {"FA707RF", "14312-6KGU80-09ba6d3e3b490c3d1dce3d16df62cd8a.zip"},
                {"FA707RH", "10197-BB5746-a958ff2eeedcfd1753c2e7151c0a3f96.zip"},
                {"FA707RM", "10198-TBWO12-e35e49d79fcc03c5cc84b5b40c222e80.zip"},
                {"FA707RR", "10199-LILM59-05448f5e429c6c4be7b531d0b6aa2319.zip"},
                {"FA707RW", "10201-0KXC4K-3dca8e95a9e676a1010efc79becde228.zip"},
                {"FA707XI", "14821-JO8WIQ-77921f1a7a523fe5674f39f10e66f4a3.zip"},
                {"FA707XIN", "14822-7W7Q8W-e2e50358e878b092fc888e8e70575985.zip"},
                {"FA707XJ", "14823-MX6CGK-0d0862cd441f96a72b1c8baeaa8aa3b4.zip"},
                {"FA707XU", "14824-O2LZZA-f0ea2bf54bbc4fd4e7391048f4a083e4.zip"},
                {"FA707XV", "14825-DUSKD1-7d0a055d869a7366105380c44788724b.zip"},
                {"FX_Series", "2174-J1XJKV-bd932ade5eebb0b2f1a36eac85bc5c7d.zip"},
                {"FX505DD", "3439-CJLCNE-3df3a90619c571de2917a35a402fe6cb.zip"},
                {"FX505DT", "3427-H77L9I-2f3f50fbbb1dc4ac6e703b2820acb491.zip"},
                {"FX505DU", "3426-9P4PXL-7e640e1e38c6395bc5455bdf838ea83d.zip"},
                {"FX505DV", "3425-31OQ2O-3ab11acfb17dca8f339fe0b8a4efa33d.zip"},
                {"FX505DY", "162-H1YFBE-1e5c82e28fa35559e5f7a6aa934d7405.zip"},
                {"FX505GT", "3423-8CCGEQ-a72ef69d8d5aab7bfd3eb34e2a55cd48.zip"},
                {"FX505GU", "3424-9J0I5H-1c209ee72d9f7ba2caa559fddd9f5fdc.zip"},
                {"FX506HC", "7821-BDCXZM-23e718ce9f58cbef286e77c7d97752d0.zip"},
                {"FX506HCB", "8452-QZYXAU-9385f6b96812173b12dcf23934d2729d.zip"},
                {"FX506HE", "7831-C0XU6K-16e393308545bf5fa327997b33edeba9.zip"},
                {"FX506HEB", "8453-42FVNT-a7c6b122bb1f2e16340c016d09b43c13.zip"},
                {"FX506HF", "14511-KKQA1F-9129e79baf1215ce451e791b51fee304.zip"},
                {"FX506HHR", "7832-KJD4ZK-d6e5fcf84caf94be5b312d6233876322.zip"},
                {"FX506HM", "7833-2EVLVM-3082377bce59838bf06bd09c3c9911fe.zip"},
                {"FX506HMB", "8454-32DRJH-818506f55eef4d13839ea8e8d8765534.zip"},
                {"FX506LH", "10917-Y6JJC7-4596433cf10a71242d9a9255c2b31458.zip"},
                {"FX506LHB", "16838-WYAR19-d3f87642f6cf12ec77e231b7d405fa61.zip"},
                {"FX506LI", "4428-Y7KRBX-df2c328780c4506a2cb2279ddf84ca58.zip"},
                {"FX506LU", "4429-34SZ6D-2df0b447ff1dcf6d887cad5c679331de.zip"},
                {"FX507VI", "14064-MTRMKH-169de48f4b0b3035315106bde1a1020f.zip"},
                {"FX507VI4", "14065-9VTCNA-b5e0ac043ffd3e3ffab879f6401b2e07.zip"},
                {"FX507VJ", "14066-W5FPY3-89180f515ba2c785b1f87c895e09cee9.zip"},
                {"FX507VJ4", "14067-DKVRHI-bc0fa5ec61259e31536356ec67d27cb5.zip"},
                {"FX507VU", "14071-EEJKDG-95e42e7c6ebaea33206a1de566588ae8.zip"},
                {"FX507VU4", "14069-1JAF2A-4d328ede6adf0796924734da3e04a18f.zip"},
                {"FX507VV", "14068-QO62TU-b6aadf3d84f816a42f111649bb1381ae.zip"},
                {"FX507VV4", "14070-WHB99Q-57b4d0c8f9405038db3e298f72474505.zip"},
                {"FX507ZC", "11454-8DEVNS-3503e698cf0b56b387bf65ebd0169786.zip"},
                {"FX507ZC4", "13785-D9YTVH-2fb9db83c6f04518f230d1d453373e5d.zip"},
                {"FX507ZE", "11459-DKP456-c8d8862da76207dc0a3bf2642b98632d.zip"},
                {"FX507ZE4", "13786-3YM16F-fa4c0a13c74e6c1253d464bbb035cbc3.zip"},
                {"FX507ZH", "11455-OQI6ZD-866eb458d7ab9ac51cb88966b2c4acf1.zip"},
                {"FX507ZI4", "14056-D01K4S-bb2cfa4d564b8a6834c78d675aebdd1c.zip"},
                {"FX507ZJ4", "14057-YY0XRG-4f7570dee1902f2012b812d50b0d2334.zip"},
                {"FX507ZM", "11456-OM12KU-7c91b2d14b093dc2eef17a555e14ada3.zip"},
                {"FX507ZR", "11458-P8RTC8-2b3802527c98749fb68397f35c6bb86b.zip"},
                {"FX507ZU4", "14058-9ZYS4E-7e244d3e7dc3641d602130563c1c84b7.zip"},
                {"FX507ZV4", "14059-VBGCNK-0314facaf02290da1a75d1b00455cb3a.zip"},
                {"FX507ZW", "11457-4F1FOO-5f24a98f90735cedf1ad9fb5ac90adbf.zip"},
                {"FX516PC", "7788-GJYPET-06cf200d5b8fb92c807de03796535e80.zip"},
                {"FX516PE", "7789-WIT1BW-b32fd9546ac9ac5b6878a3f46c30247c.zip"},
                {"FX516PH", "7237-G94WZH-30c10f820dcdf03cc661fd9b73c2927a.zip"},
                {"FX516PM", "7790-OG0DVJ-e7ad44e2fa38eb4256e09a62a744659a.zip"},
                {"FX516PR", "7791-LJ7RQ7-36c58a0b55aeb5c3782302fd58139815.zip"},
                {"FX517ZC", "13757-QJDW9D-ab71bd2ecf24d182bf32fbd931b24e58.zip"},
                {"FX517ZE", "13758-75WLCE-fac3186e372a6e0e422d3d78de1addb3.zip"},
                {"FX517ZH", "11461-A4BH34-2464f8a583b064ab2bdb336dc13d8b2e.zip"},
                {"FX517ZM", "13759-694BOG-7b8f61a1d8a387715721fd2b8e0bd766.zip"},
                {"FX517ZR", "11463-MCHEWS-7a732705aea49eb663fe1e8930c463de.zip"},
                {"FX517ZW", "11464-S41ORA-d99633ecf421ba4ab73800bc9ebf2e5b.zip"},
                {"FX607JI", "17739-YJBJMA-59c984b28eeb6c02498190f768b12b52.zip"},
                {"FX607JIR", "17740-78V0QP-43bf1ea30f94d3d3a286f42dd90a1816.zip"},
                {"FX607JU", "17744-JEFX0I-f1ee2104c17a5cc511efefa3dc7b9601.zip"},
                {"FX607JUR", "17743-A2JS5H-224a1f32f7dbfecb0d6b91fdb6ffac8e.zip"},
                {"FX607JV", "17741-OWALZC-eb293a03c7b4595270939bccfe891656.zip"},
                {"FX607JVR", "17742-3OOC7N-f28635b55ce56f9e32672b2e409a075c.zip"},
                {"FX607VI", "14072-0Z0LX5-c3145816b5c9b045a3e829e194425624.zip"},
                {"FX607VJ", "14073-H1KA78-95472184994955393ebf635af4a55c71.zip"},
                {"FX607VU", "14074-RCMD10-b606bf5fd03bdf6a8aef412202de86ff.zip"},
                {"FX607VV", "14075-BX4NJX-a9e24be091f08e6cc3885397f143bdc5.zip"},
                {"FX607ZC", "9267-SQXQMT-edf1ee77d7f276d9b2f14fbe80e86459.zip"},
                {"FX607ZE", "9268-MI06A0-5f60b76f497adb2c80dc5d34e7ca00ef.zip"},
                {"FX607ZH", "9266-75PO51-12f649be993639c1ff986129977bc516.zip"},
                {"FX607ZM", "9269-MEPP17-baf25a9abae3ae712da40dc6c4f2de22.zip"},
                {"FX607ZR", "9285-7B20C5-9a0ee909d954eda9fd9f334a6b76bdda.zip"},
                {"FX607ZW", "9265-9H05OM-5b2735efc15db9c49302451b728338a8.zip"},
                {"FX617ZC", "9263-3PLDC1-74d554f08fac5b1689c7dbb581fc97c4.zip"},
                {"FX617ZE", "9264-9L5V7F-203b8f8a48d47470aef80dd42bbef60b.zip"},
                {"FX617ZH", "9261-DIXTFE-b20887acff77b59a80e62088caf695f3.zip"},
                {"FX617ZM", "9262-OXO77E-8476443cfd885a718b915731b9807bc0.zip"},
                {"FX617ZR", "9286-VA9VXJ-8c69c08b5812b00656142d93cb5a5013.zip"},
                {"FX617ZW", "9260-O8UGYN-9678cab288d9b85ec59961647ca94ff7.zip"},
                {"FX705DY", "163-N6IM0Z-3bee0b29abd2d6b419ba322661d64a6a.zip"},
                {"FX706HC", "8834-3HMY7W-fb49a16bc9cda208a3acf5f2ea32fc63.zip"},
                {"FX706HCB", "8835-FHPH4O-319d35ab96a1eef3b5ae7fefde106cf2.zip"},
                {"FX706HE", "8833-SW8GIL-71ae2ebbdb18ec3380cf682be0ca32a3.zip"},
                {"FX706HEB", "8836-SUE4HH-55876358bbe39f53ea2010c8f66e9093.zip"},
                {"FX706HF", "17052-8B8C18-0c762fb15cd6367d08c954ce5e01f497.zip"},
                {"FX706HHR", "7837-S711RT-ed9d8a61e80895a6403dda19ac873f62.zip"},
                {"FX706HM", "8832-O7W1D2-853dd8eb62665c369cd2c3b716a53b80.zip"},
                {"FX706HMB", "8831-NKXZ56-437ac6d56280dd0734f9f56db3ba6a6f.zip"},
                {"FX707VI", "14076-C4CQ2E-0771f0ec46072ac38a7dbb8829783298.zip"},
                {"FX707VI4", "14078-UXPPY2-ffe538cd58da15c780e6263c2818f995.zip"},
                {"FX707VJ", "14077-6KB0BJ-af4fae81000b0143477091deaca569da.zip"},
                {"FX707VJ4", "14080-YBM0VD-bef842b5fcc1b49cb054cfb1eb3d2258.zip"},
                {"FX707VU", "14081-I8JRY3-2ab8a5dcab2033a19de169e424da1530.zip"},
                {"FX707VU4", "14079-TALDGM-c7d6c6820a223967463a76152e178af4.zip"},
                {"FX707VV", "14082-2UYJNX-a21d4e4e856e8a4fe52b30593705d818.zip"},
                {"FX707VV4", "14083-79JC8K-74d96cb066ded2d86255755fdc1f3167.zip"},
                {"FX707ZC", "9278-7TRFVL-9a0693a78d8d69a265cb7101314dd055.zip"},
                {"FX707ZC4", "13358-EPBJ9S-035eca4acee4db9814f693f4aeef0147.zip"},
                {"FX707ZE", "9279-QYJXB7-17ff65bda614791358426882d48be6fe.zip"},
                {"FX707ZE4", "13357-V3LZL7-64a1ecac6b4657e98a422cd5657cc89b.zip"},
                {"FX707ZH", "9280-62FAFQ-3193a4215440ecbe7fece9091fad6205.zip"},
                {"FX707ZI4", "14060-EEB7IW-e716f0f9a1f8f91e5ae93d902de96385.zip"},
                {"FX707ZJ4", "14061-T7UBPM-91f62b48749f5ed42e3c3d1f2b8bb645.zip"},
                {"FX707ZM", "9281-YKO83G-ff6a896b4e055cc6500470636cf9e1ad.zip"},
                {"FX707ZR", "9284-GYDQK8-bfdc718699fb7030b0f133402933e12f.zip"},
                {"FX707ZU4", "14062-52EB12-8a85d0170af52dfbaf216b98085015d0.zip"},
                {"FX707ZV4", "14063-R6EM57-62ad692f855e33bb05aaae1823207415.zip"},
                {"FX707ZW", "9282-E6GYQU-5f673e25b2ef5e57c78e36668e9c5e9f.zip"},
                {"G_Series", "2176-RS8S2K-e815e5e0be51e1bd76674cd3826cbfec.zip"},
                {"G512LH", "3312-AM98EE-e9a32238fa66ffe8683ff0a34019f216.zip"},
                {"G512LI", "3310-9MDL49-daee5dd162a699f60f2bb68a3cba7ef7.zip"},
                {"G512LU", "4150-SX012C-b6e7d21b645495f57b2fb7bd9734e839.zip"},
                {"G512LV", "4151-LUEHCJ-f145d1dd67079a3739a564cb04eaa6ff.zip"},
                {"G512LW", "4152-4IMGHN-f0a946d2f21860039a9181b17ca1f1be.zip"},
                {"G512LWS", "4153-IZDAY7-3cc48027800d6d123db4f584a5a2e4a4.zip"},
                {"G513IC", "7233-I1XU1L-57f580817efeb347c2e64b467b569d48.zip"},
                {"G513IE", "7232-7C4QK2-effb8e534fc4639d7fd72e957e1a13fc.zip"},
                {"G513IH", "7231-YGY2HL-ef7e448a0e3b2502219c74c2f8a26a72.zip"},
                {"G513IM", "7883-JSH1ET-04fb928a7fe54481b8156627d30451ce.zip"},
                {"G513IR", "7884-DB01TN-5bdc2457c9a42ad9c0b1ab82139beeac.zip"},
                {"G513QC", "7228-I73APA-79e1e7f9582bdc44f510d651e909250b.zip"},
                {"G513QE", "7227-9J3H2Y-d222e0119fef169bbe0f20f754743ca7.zip"},
                {"G513QH", "7226-MUU2SQ-05a65cada4f1c986b7b2c5e2cf7ea796.zip"},
                {"G513QM", "7882-2FWOWK-ca956a4c309edb8cd9d9c24178ec3912.zip"},
                {"G513QR", "7885-4VL6WK-6a21c044bb2d69f55febdb6ffcd5b7d8.zip"},
                {"G513QY", "7881-S6R4GZ-5832d1de6fc107509803927e4a52ebf7.zip"},
                {"G513RC", "14513-WMQ0GS-4018fc7459d96efec72f15f724c33062.zip"},
                {"G513RE", "14516-VXJYUR-fbaadee0ba8dea2a24282b2be882d571.zip"},
                {"G513RH", "11732-8ODACG-845738811f5dcaee76c7ba113ed0c8bd.zip"},
                {"G513RM", "11735-UXEJT9-03457f6d6350f0a860fe6386de27a659.zip"},
                {"G513RS", "11733-9SABRD-3a4c8dee5ff96668dc9ff509d626e17e.zip"},
                {"G513RW", "11737-VS9IFQ-b9c6657578263ac3ab1062b32ab599bb.zip"},
                {"G513RX", "11736-CHOX6M-ca19a5d6e7153102c3825f8b3371b4d7.zip"},
                {"G531GD", "3698-YSDO31-352980933199182c587573ecf056711b.zip"},
                {"G531GT", "3697-OYLYLQ-4916a89afb3a6964cb2b0ddd8dc45fe1.zip"},
                {"G531GU", "744-S7QQP3-8572157fa95e0eb235f29df0227d5064.zip"},
                {"G532LU", "4146-NF3MBY-79c262fe416eb469fdff811d501233c9.zip"},
                {"G532LV", "4147-F6P59K-328b1796ee854653a1470be865e06d0e.zip"},
                {"G532LW", "4148-KUJ0SA-affcc286138b433b9b051dffc8158672.zip"},
                {"G532LWS", "4149-RDJKG0-c5f4470225e184cdbc34b9b41c9e2666.zip"},
                {"G533QM", "7880-VL41LZ-ea6868a4918e7a2571a169b0b20d53de.zip"},
                {"G533QR", "7879-3915UU-cec5ce6e895bc9acf0367b1ae69d7d01.zip"},
                {"G533QS", "7878-SLEHTN-4844d16bf258768105873010c17398ea.zip"},
                {"G533ZC", "11725-BVSBSP-1f91feb49662e4f4257a27ea081db4ac.zip"},
                {"G533ZE", "11730-DEG4Q9-3d77f3901752747e449fdaa5467a250b.zip"},
                {"G533ZM", "11726-01FCGK-180459312539e4c38def3c7323a60494.zip"},
                {"G533ZS", "11728-KGTOE8-4fe0474fc6e8944dbdcf726bad1a7db1.zip"},
                {"G533ZW", "11727-98BKOJ-93e373e64e92c21ff018ba6c66a9de9d.zip"},
                {"G533ZX", "11729-XNFMPR-e0aad4e4d4956dba8629f419b1f86026.zip"},
                {"G614JF", "14496-NAATJZ-a3847e890e57aa749645b854b28ead79.zip"},
                {"G614JI", "14168-4VTU0U-37decb2df809672eccf1156769eda929.zip"},
                {"G614JIR", "18120-52TT5M-28a5c8e138ed1660e9eadc81121f789a.zip"},
                {"G614JJ", "14497-P0D5PL-abe178509e0316da9ee56b24e15d0384.zip"},
                {"G614JU", "14498-5D71EB-2300be03087494ee34356470ca022b7c.zip"},
                {"G614JV", "14500-SPJCAL-a8e2e223857c334ba4c20c8122151bb7.zip"},
                {"G614JVR", "18121-2WOF0H-4e7045c7a210693fef2b99b8e032316b.zip"},
                {"G614JZ", "18199-P9WWOH-093d361358629a2bddd5bff7b5fb7a5f.zip"},
                {"G634JY", "14163-JLRBOO-8ea519425efe56cc144e38d22cebd3f0.zip"},
                {"G634JYR", "18087-6Z1W8Q-cd92004b6b9392e2cf07317db8ca9c6d.zip"},
                {"G634JZ", "15827-MSW5I1-0f39e92313e54afe3e35f5636dc37fd5.zip"},
                {"G634JZR", "17891-O7PAGL-5c6e6d059ff7bc1b9245fdf8a8066263.zip"},
                {"G703GX", "105-NMT9XG-76a0810ddf16ee376fa049bd179377fc.zip"},
                {"G712LU", "3364-3GYELH-a0f545d6a2f5ca2516e91f0495f98110.zip"},
                {"G712LV", "3365-UYDEWV-6675727e4e0588b95d9faa2df69b1972.zip"},
                {"G712LW", "3366-OK41IE-fc01da1e3d6217e6bce24f3237b5631e.zip"},
                {"G712LWS", "3367-N1B9F6-72dd79495de9b660a8b120579c708623.zip"},
                {"G713IC", "14187-GKTF5N-c602fb66b48f86aa2ab7b14367dba7b0.zip"},
                {"G713IE", "11023-PVEVI5-bfa9a58633213b87629c5117030d54f2.zip"},
                {"G713IH", "6317-I3AGUT-a0a1e3b68265d7bd3a2a1657afa0cfb6.zip"},
                {"G713IM", "7890-IC1UIL-85b242f2f61aef5cd16166178b6a1adc.zip"},
                {"G713IR", "7891-832AOR-104e408922a3f0c4e8d97d03dd000083.zip"},
                {"G713PI", "14527-XK0JY1-ffe9b04acfd36ebcf3b1331577515b8c.zip"},
                {"G713PU", "14528-AC10R7-ee30e2f76e16c16db44758ac911e9d61.zip"},
                {"G713PV", "14530-3ONUVM-abf27eca2271dcdfd150acdf6cc26800.zip"},
                {"G713PVN", "14529-YMPI29-b958f3ae6ad8118997c16c87d9840fb9.zip"},
                {"G713QC", "5496-KAW7ZL-7a45d99d71ba375d79d047a015c27c2a.zip"},
                {"G713QE", "5497-T7QUN2-f5c00e128ed96b9727dcdfb4d857dbb7.zip"},
                {"G713QH", "6316-ANOUE8-9b18e7d3aeaeb5cc4520be09df860617.zip"},
                {"G713QM", "7892-JCIYF7-acf05d6b34e417efed385e84dc079677.zip"},
                {"G713QR", "7888-XIBCC9-d2e608529ce49f0daf90bf3d81cbd69e.zip"},
                {"G713QY", "7889-FH9X9W-5c4d00f5506924fc9f29e1c190b1bf51.zip"},
                {"G713RC", "14514-8R6TFS-eb95e08804622256fc5be85a1f14301c.zip"},
                {"G713RE", "14515-LTB9LQ-a6a522a9d63083e0366d13f40af54ebf.zip"},
                {"G713RH", "11305-VO5JTS-7cb1b763f054da29ce2ba1e08c897a60.zip"},
                {"G713RM", "11309-GX2AYR-5b172b743c185242080b08dd7e2b7fc7.zip"},
                {"G713RS", "11306-E1A3OM-37679d1dfc144d892325420d784f5d8e.zip"},
                {"G713RW", "11308-IPZSAB-6c8827c63a27cee6257c755bf0bd9c41.zip"},
                {"G713RX", "11307-JGFO1D-cff83a2f34e3c0ed755ed63c20d2905d.zip"},
                {"G732LU", "3368-C89MVD-1ecb4a036b697bd4bd027d2bd160d219.zip"},
                {"G732LV", "3369-TI19K6-56ca5eb37397117c97e0bae42e82b2b4.zip"},
                {"G732LW", "3370-HLPSUS-9c7615d0220bde50de5ecd4c7e51182c.zip"},
                {"G732LWS", "3371-B4CG89-c22a9f4deb49d6e1e921ebd5a0cab685.zip"},
                {"G732LXS", "3372-OJEP07-c0e21f578660bec502aec19a3645b067.zip"},
                {"G733CM", "11332-V8B7E9-2a7f61f88106051be62b57250ed33eaf.zip"},
                {"G733CW", "11334-MQ3PJE-f03f2950247ac635dda1b30245d220aa.zip"},
                {"G733CX", "11333-XS984I-d48bb5ee3416d47fa203090bc8bce04d.zip"},
                {"G733PY", "14531-B4NFUU-4ec97a54081ea4b713975fd1ab7c1b80.zip"},
                {"G733PYV", "15178-1CIVEB-5a9d09a4d22d6361992c7cd6ca27fb41.zip"},
                {"G733PZ", "14532-4UUM5E-296ee92ca5416a8c59bffd6f101feaa5.zip"},
                {"G733PZV", "15179-EDHNHX-ce69d260b1e8077d8d522d3b4326322c.zip"},
                {"G733QM", "7893-OW1O2I-a2dcb8ff88cae20c7086bac8d9312dbc.zip"},
                {"G733QR", "7887-OA7Z12-bc86110ff0d1f359d9230fca8bbabe51.zip"},
                {"G733QS", "7886-V9PIMC-27ca6272c2b093264d88a32f1093a4fe.zip"},
                {"G733QSA", "5501-LX7POZ-9289c2079bdb48e2fdd76d7981fb0e5d.zip"},
                {"G733ZC", "10077-J3XTHP-525deb09f2ada7dc36dc82772f7a9cce.zip"},
                {"G733ZE", "10079-KAW34P-16c4b5e812e10019f21b4d7f879acfaf.zip"},
                {"G733ZM", "10078-K6B3CR-63dd00f6ecc3eb61e92439ef5caaea17.zip"},
                {"G733ZS", "10081-VM2AN1-517b1e41388f12048ec3a88c780f1d65.zip"},
                {"G733ZW", "10080-HY42QH-aa285046069cfe113953b91f1a28d101.zip"},
                {"G733ZX", "10082-JS5F1Y-41bccc831aec7bf1c0a59a309bc607c7.zip"},
                {"G814JI", "18243-BAU912-671afb033d3b1c44f9852ff608910f42.zip"},
                {"G814JIR", "17315-02XE3O-7a00ed5b91bc305aade5a1830691d0d6.zip"},
                {"G814JU", "18193-Y5GMF0-843cee9ed982a3d5aba21e99efbc396c.zip"},
                {"G814JV", "18194-9Q537I-c5c2ca782d26dcd5346c110767347903.zip"},
                {"G814JVR", "17313-P6P61Y-5960dc1d0bd8ba2057bc22aaa8bf89e5.zip"},
                {"G814JZ", "18195-V98WBX-19d4c2e52916bf9c96fcaed6f1d9bdda.zip"},
                {"G834JY", "15954-HAV3KR-d219c43750d485279e12cf76f5b2569b.zip"},
                {"G834JYR", "17314-YYBQF3-a65b1bc3fd1e8de145dc9d606e8c45c8.zip"},
                {"G834JZ", "15953-C7XC62-a987058c0a26fa3c929b300d099296a5.zip"},
                {"G834JZR", "17316-AFT379-743360d9b36031d91a72fe03e3fafca0.zip"},
                {"GA401IC", "5864-KC8TTP-082f9a62dda322ccaed82ff3e3466bf8.zip"},
                {"GA401IE", "5866-JO8504-dde5402bdecafc1cef55a7dc3d6167ec.zip"},
                {"GA401IHR", "5865-7DV6TB-9833a54c7334dc3aef1b9e99f56b9e95.zip"},
                {"GA401QC", "5129-PY283Y-ac372208c57a7f558db6c169cd50d091.zip"},
                {"GA401QE", "5130-C2G5CO-8ea0eb1ca537d77d19bad13dee55294c.zip"},
                {"GA401QH", "5124-QQ97GA-bbde59c5235d8b72b78059b95e8a23a2.zip"},
                {"GA401QM", "4922-222SBA-340a82b955a5f57ff39f6d2d08af365b.zip"},
                {"GA402NF", "14160-ORONGT-ca84f0561683e1a07548d0cb7e42c275.zip"},
                {"GA402NI", "14159-SDCMA5-300bd29af08b0f5d1527a375f8c326f2.zip"},
                {"GA402NJ", "14158-O7H0GR-10db546f9901ac02d3ce8d5a28cb4772.zip"},
                {"GA402NU", "14157-QJ4W59-69222faad499f94b4d96f7d85504930d.zip"},
                {"GA402NV", "14156-ZKGYBE-ae1ac3947eb8684cabc0cd45f8d4c67e.zip"},
                {"GA402NY", "14155-AL3KZU-17ac9e4b88c03ebaf983134f76f9a4b2.zip"},
                {"GA402NZ", "14154-SY6J4X-7d8416a09b51e32f856f3353173ccc70.zip"},
                {"GA402RI", "10145-9W1623-272e082c57c6e41074b2cace72adc92e.zip"},
                {"GA402RJ", "10146-IY7ENY-f6d2d25b5b6b21616ceddc4e9aefa656.zip"},
                {"GA402RK", "10151-7RPIER-2588aa83188168f8375ed9bab7bb67d2.zip"},
                {"GA402RQ", "10147-9NQMQH-7ff3944c376e91b26c0a1c18122a4262.zip"},
                {"GA402RU", "10149-3F10IX-43c96d30b6702261c3cf85467857eec9.zip"},
                {"GA402RW", "10150-5H5ISD-1badfa856f0ec8d15d9d5832a8d54fc9.zip"},
                {"GA402RX", "10148-QFCU8O-704d371255ae8a72737f5f8eca75eb9d.zip"},
                {"GA402XI", "14899-86ZX57-5940ed56b182781334ca2856f6ba0b9e.zip"},
                {"GA402XJ", "14901-MDIR3C-5022331539bb187bf041a7e3ab0a46bf.zip"},
                {"GA402XU", "14900-KYCOC7-361bb364d26ff834d6bd716d14f70006.zip"},
                {"GA402XV", "14902-EL34M9-731ba1cc0d27d2db9a79d872673e7958.zip"},
                {"GA402XY", "14903-ICC56W-0456739d8ba5ee543dfba99eb6ad217a.zip"},
                {"GA402XZ", "14904-B3K2NR-928b7272790e7da0079c549cd4885d5d.zip"},
                {"GA403UI", "18084-XTMSW5-6cad333378f3b1059688a419a8422fc1.zip"},
                {"GA403UU", "18086-WXV037-ad90bc3f20ff056c45dbbb848fa72a67.zip"},
                {"GA403UV", "18085-G56QVI-3f24b5bd1eaa8dd9086b86092cb62ffb.zip"},
                {"GA502II", "3554-REAPZQ-5f94c7ecd52ae2cad50aaac4341bec7b.zip"},
                {"GA502IU", "3555-15UWZ4-d1cd040265773efd61bdfd146140a990.zip"},
                {"GA502IV", "3556-ULLH3K-1f56134e43e1ffa353256fbeb9840d3f.zip"},
                {"GA503IC", "7457-RENJRP-5b833a2bd8e1e13ec41f0acf7ef04fd0.zip"},
                {"GA503IE", "7465-QO1Q0F-663f0c9501e649bcb0069413f3081a49.zip"},
                {"GA503IH", "7458-D6CHF3-4c6b83cb3efc11713fddd9c41cfd127c.zip"},
                {"GA503IM", "7741-F8GHPK-63e17aca55a11e838b1079eb0c6bea16.zip"},
                {"GA503QC", "7459-308BEA-bcf2604511112df5fa810faa9cdece89.zip"},
                {"GA503QE", "7464-4T2J1D-3920997d2f44d3b40dffd6ba9f028075.zip"},
                {"GA503QM", "7739-BSD39S-f9858fd4a68ff8ef60b8abf2cda92dd7.zip"},
                {"GA503QN", "7740-QT19P2-33054dbe06ae3baa435a44ce529eddb5.zip"},
                {"GA503QR", "7738-QVCIRY-4e612ff997f11448c77068e39ee0681e.zip"},
                {"GA503QS", "7737-B5L4LD-4400a4ddc94eebefb0678fb281b63fa0.zip"},
                {"GA503RJ", "10630-MSC4Q2-f16f1ef2847069e50aad62083836b220.zip"},
                {"GA503RK", "10631-VXWRAN-b2a1aa499bf33f1f9bd3d80e31dd9d69.zip"},
                {"GA503RM", "12054-38JGOX-35e7b1afb150daea1eab90ae97f0da7b.zip"},
                {"GA503RR", "10632-IOY3C0-bf16d1028f572ede9dd8e1a2fd632ae5.zip"},
                {"GA503RS", "11745-689QTC-6c441a7161a34b871918fce5997c4e2e.zip"},
                {"GA503RW", "12055-SG6VTH-91cc5782da63cf3dcc16804a47db25a2.zip"},
                {"GA503RX", "12056-MR9J0D-b7dad63f751d2e53aeee479524986bdd.zip"},
                {"GL_Series", "2167-P7ELNB-44d357ee720a0c48c2eb37eee785316c.zip"},
                {"GL503GE", "57-YI7HPS-9ed36ba45ff6398f3648a44ced4e5e61.zip"},
                {"GL504GM", "58-JOZ3S5-998677afd41375cb8fd60ace8d11a788.zip"},
                {"GL504GS", "59-KS4SJK-b1c9807c7d7dc847fa895516b3b9a8cb.zip"},
                {"GL504GV", "167-OX51EF-b8b5c713bff267d315e0255370956e75.zip"},
                {"GL504GW", "168-NMGNB6-1f6d79b87d077c235c57d4505ce802e7.zip"},
                {"GL703GE", "60-M0UOZ3-1474ef8209bf010de4c6e6ae94ce351d.zip"},
                {"GL703GM", "61-WJGLV3-b8005e077aa4825799459412849e346c.zip"},
                {"GL703GS", "62-HU12J1-a88023229e3a2dbc5fc1aca1b07e4d76.zip"},
                {"GL704GV", "106-3IHGVF-5d15db39c04a44e3503d3536d4f0c132.zip"},
                {"GL704GW", "107-NZ7VZL-cae8497c2eb8479f4808d2b61e7c8e70.zip"},
                {"GM_Series", "2169-K05CUW-841a5508c96649de28d613e32dfb7fcf.zip"},
                {"GM501GM", "63-3CL0UZ-eee6728dd2517d6e3af581f1caef0114.zip"},
                {"GM501GS", "64-GH66XC-5e69efa00bbe486efd897414ed329545.zip"},
                {"GU_Series", "2170-FYLCER-f45e5d5e38121b7bfe275b780e739a8e.zip"},
                {"GU501GM", "65-LSRZXK-658dc6e1c89a7787201ec7c74eebf2d5.zip"},
                {"GU502DU", "4004-T40IY1-74f962c9d9d27544c964bdb347074472.zip"},
                {"GU603HE", "11674-VGXXML-41035c5f9dbf4e0b64a74eb3f89d95e4.zip"},
                {"GU603HM", "11676-GO5KVF-581b4f49e0e12d9d6654ab2c1db9ab11.zip"},
                {"GU603HR", "11675-AZPXHF-48a9979ce428acfcc57c4e295f8a9840.zip"},
                {"GU603VI", "14579-CUHZFU-ea9e6cf216e1359b0f3e300e137720f7.zip"},
                {"GU603VJ", "14580-18G1FP-4456270d80c37a0597b3bfc559222f64.zip"},
                {"GU603VQ", "14581-8YY8TX-e7aa10a20a41245500f40841b2298a42.zip"},
                {"GU603VU", "14582-9I3GQS-564dd1ba9ee37dc03bef022bd2e29049.zip"},
                {"GU603VV", "14583-XH4I69-879293a1a305dc952afb15b481722e09.zip"},
                {"GU603ZE", "9817-3EENL9-3187794df7ec8ddef2fe04d0787eff63.zip"},
                {"GU603ZEB", "9818-KOP3C1-11ab717fddedafb9066b362d8da5af85.zip"},
                {"GU603ZI", "14431-H4XZCU-1c907d4e22bbd5000a6c81cfc2e1dae4.zip"},
                {"GU603ZJ", "14432-Y5HC6R-b50edb7a5ad9469b89c35e5cd9fc114f.zip"},
                {"GU603ZM", "9819-718DH0-30bf094253e5178aba765436bb8b150f.zip"},
                {"GU603ZQ", "14434-3KPJU7-211e518352b0660babc4816fa5dd80bd.zip"},
                {"GU603ZR", "9820-LIQ8FP-6a60aee97d7028e7ea63bd2ab6c2b099.zip"},
                {"GU603ZU", "14435-2ZW8FL-65ab99a80fe08d8d088d8fdc998076f4.zip"},
                {"GU603ZV", "14433-FEY59U-cd0508e779a6b3874a61668f39ff78e0.zip"},
                {"GU603ZW", "9822-3R2PGP-f2d24c6931e733a5e0e41dc8560b80cc.zip"},
                {"GU603ZX", "9821-AQYG62-f42f167ec7b4f4fe003f05b1c1c61d58.zip"},
                {"GU604VI", "14284-R38BM3-e2591bfa2582ce68b2c8aba63beac5e3.zip"},
                {"GU604VY", "14282-78N7V4-25e76896744c53317fa41a80788a2241.zip"},
                {"GU604VZ", "14283-U4JX5C-f31dff75d52f48ceca0102fd1bc76e31.zip"},
                {"GU605MI", "17935-R3OAJH-55a4fbb2be67a37226accd279357268b.zip"},
                {"GU605MU", "17936-VBZ5PL-ad3a380f62bc87ee738c09c8e5255f91.zip"},
                {"GU605MV", "17938-7DGM0T-caed792b162e94846167888f36fc632e.zip"},
                {"GU605MY", "17937-HZWNQ4-72d9c4fed1b31f188874cd6461ffba83.zip"},
                {"GU605MZ", "17939-IEJ2X5-fe077100dc4eafbae46d7eb831a791a8.zip"},
                {"GV301QC", "5128-RN42GR-9a6049e7adabd1ff2aeed24293fe03a8.zip"},
                {"GV301QCZ", "6778-152PHS-7a9ad124aef1059471542dfc8912ed7b.zip"},
                {"GV301QE", "5125-PD15GR-9a7f72cbf5bd97d283464d8e407f0231.zip"},
                {"GV301QEZ", "6780-N3Y08B-222db095847d99663a575be36fbcf410.zip"},
                {"GV301QH", "5077-0BEYRT-ff8e30857ab4af017381f5703b39abed.zip"},
                {"GV301QHZ", "6779-9XZE32-329974b938dff4baf3097e3d745839eb.zip"},
                {"GV301RA", "10272-1B7YAA-45e3697377ff2e7dd68c57f5919eecd2.zip"},
                {"GV301RC", "10273-UJ0Q76-53bdfe00929ccf3f594f813ba235438b.zip"},
                {"GV301RE", "10274-1J3P7N-42dabc435e4daa40d4be2fa482624321.zip"},
                {"GV302NA", "14663-N4C4U4-1654950fbc1fad0e37a016c4a1e8a934.zip"},
                {"GV302NF", "14668-GMKBGC-1bd7957eb1a688f8eb0e6fb85395b839.zip"},
                {"GV302NI", "14664-ZQXAM8-1dec6b07c9589698c8cccdbc3a3211a6.zip"},
                {"GV302NJ", "14667-OFKJGT-60db594b4af90c481212ee31ffa96d38.zip"},
                {"GV302NU", "14666-6M0N3K-3d08325f6c4b3fcacf53a2f863a0e6c5.zip"},
                {"GV302NV", "14665-DWLIIQ-9b3b8b0aaca3a071f82cbb30fbbe2fd1.zip"},
                {"GV302XA", "14934-AU5P0J-1873c4e95dadcf72b1fad550108a0eb5.zip"},
                {"GV302XF", "14935-D5PHI6-aa9715b2328c499c8cd72b4958ba6641.zip"},
                {"GV302XI", "14936-CHYX8S-f5fe72833a34f1f002dae6789f2760fe.zip"},
                {"GV302XJ", "14937-LMH57E-651161fb27c3ef9904e15fc42e5d8352.zip"},
                {"GV302XU", "14938-4DXJJV-82feb9f0156bb914ccbde2de6d59ad14.zip"},
                {"GV302XV", "14939-KLW56G-32ffda0b7ee35bbe4ca5789da7210cc9.zip"},
                {"GV601RA", "11078-109U4J-33e9d7342819b8a7a534632df659e509.zip"},
                {"GV601RE", "11079-U5MUWC-8c22738f13a798376733fead5f091a4b.zip"},
                {"GV601RM", "11082-XN5PLH-5c405808680f6bfc36c5069bcd5dc366.zip"},
                {"GV601RW", "11081-FK9JFY-b82e25b8f9bcd2ad1d3ae23c7cb327d0.zip"},
                {"GV601RX", "11080-45B7E2-b64fdbf32877dc996b4efe274ff51f41.zip"},
                {"GV601VI", "14446-JS3QXL-72327a76c47c2ffdeaad392197a4b2ce.zip"},
                {"GV601VJ", "14448-TIX8CU-ed84b2ee1d92d2f38a891ed1c684eddf.zip"},
                {"GV601VQ", "14447-7C8UIX-13cc4d4632406ca5976358a4080f26af.zip"},
                {"GV601VU", "14450-0SP39K-8071216d3c679d013fffe57eca9f790c.zip"},
                {"GV601VV", "14449-QZS5FH-6a7771cbbf9bcc641a5418f250ef3b43.zip"},
                {"GX_Series", "2171-LHV7R6-bf1d918fd26714f10c3f3de990e30a1b.zip"},
                {"GX501GI", "66-U9BQN2-9a12abd42e07169ec6ebf4d4a6d2c148.zip"},
                {"GX502LWS", "3552-YRITSK-6f08012703e7718be25477f20ccd4d7e.zip"},
                {"GX502LXS", "3553-3ACO9T-20757861512ff920da316c949249b62d.zip"},
                {"GX531GM", "67-ER25S2-71ec74e4e8d14fc9c6cc8608d1616e5f.zip"},
                {"GX531GS", "68-V9PHXL-3cabb33363337795287458e3b10114f8.zip"},
                {"GX531GV", "108-VCV90K-00cec46bd49df326109c060367eaa57c.zip"},
                {"GX531GW", "109-0GH5PV-ce1ff55527d5da0b547df776af887276.zip"},
                {"GX531GX", "110-A0TZ36-4cf1c1cdeee6e0e27567e317444c62cb.zip"},
                {"GX550LWS", "3671-DEEX56-d93255f509e16f740c71e8c358e8ff36.zip"},
                {"GX550LXS", "3672-O3V2FT-6572c1aa661e3f406ce6e840075d81ab.zip"},
                {"GX551QM", "8108-47QAG6-7d57f542a91fca797f6bad910af5b864.zip"},
                {"GX551QR", "8109-ADSI6K-87224022e84f6a6c0ed2820cc4642a66.zip"},
                {"GX551QS", "8110-NLPALF-6ec9938f67229be071cdb8c436303734.zip"},
                {"GX650PI", "14591-ODJA1G-23aaf98034a279ae87f93f67db9b3b95.zip"},
                {"GX650PU", "14592-A6IJIM-e847943987f3cf4f1d0278fe1d5ee984.zip"},
                {"GX650PV", "14595-XKP10A-e14bb5df924255dc965bb975e1d64434.zip"},
                {"GX650PY", "14593-KC9ZMM-6e519dbcdd0139ed1d8e7649e407d8a8.zip"},
                {"GX650PZ", "14594-AETGOJ-4ba96caa5ae0ee68d8b457e0fbc03a17.zip"},
                {"GX650RM", "11320-E8SQ6M-5673cb197e000111c0d406b1156d44af.zip"},
                {"GX650RS", "11321-UEO888-cbff96301261591d9130980fcc5950e5.zip"},
                {"GX650RW", "11322-YIN6KH-2a804a1dd4da59662e6746f83d636452.zip"},
                {"GX650RX", "11323-64AK0P-9929933ff3fda3dcabfef096d7242a7c.zip"},
                {"GX701GV", "166-EI8VY0-1568b74c5bbf22297720b548ce6bcefa.zip"},
                {"GX701GW", "165-C6KON3-9b95db9f024e457420793b4915749fbd.zip"},
                {"GX701GX", "164-I4CRXM-b5e8ddc68c884eda5cf4676dd2683624.zip"},
                {"GX701LV", "3309-DPFMBZ-ffda0b00117a5f1c69a091158b631bbe.zip"},
                {"GX701LWS", "3308-PQVG48-ad364af358f946382f840c969c73d49b.zip"},
                {"GX701LXS", "3307-AY5DMD-681ac8295b6f98fda4c66b891f19be4d.zip"},
                {"GX703HM", "7962-KBDQY2-ff82594ee47f5c1058bc2115b6a74841.zip"},
                {"GX703HMD", "7963-A3TM3L-5f39a856e3f694e9db1ed5a5410baa39.zip"},
                {"GX703HR", "7967-5LQ4FV-7b6af6e1dc3a6c0786bec0ec084ee5c2.zip"},
                {"GX703HRD", "7964-TFYJ6L-4bec23647da2f9bc8403d060635dbdda.zip"},
                {"GX703HS", "7965-3HSRRO-c1869c30564e2db44a97c1e8953093b9.zip"},
                {"GX703HSD", "7966-9CCDGL-96f0c0f9e1e50ee66894e7b168e59cc3.zip"},
                {"GZ_Series", "2172-Z8OHBD-393c5060aaa27883e2fcbebc7e53997e.zip"},
                {"GZ301VF", "13604-50UADT-4df87ac6eab6ac261647875d727db9d4.zip"},
                {"GZ301VFC", "13787-WB73LI-b148b7cf3c90bc90fca4938bfc6fbbe5.zip"},
                {"GZ301VI", "13601-DPC6YI-17ac748ec814ec3152ad973f3a0cff4b.zip"},
                {"GZ301VIC", "13790-Z6V8RW-7a4f735f65c545158c7ba715694aa3c7.zip"},
                {"GZ301VJ", "13600-UNQHZ7-e7d0cbf757eeed28dba9295b517ef835.zip"},
                {"GZ301VJC", "13788-QVWJWQ-a13027f81094f94eea1d86152fe69ba9.zip"},
                {"GZ301VQ", "13789-K6I7SO-4a8594363610c383e39940ff135c0d7e.zip"},
                {"GZ301VQC", "13791-DBRKCU-a10ec3e290c5dc93bbb45f52c714a9b8.zip"},
                {"GZ301VU", "13602-6JQ2BK-4353d6c0b83589477ff4570b49258217.zip"},
                {"GZ301VUC", "13792-009F1L-4ee6b0014eca72c7d0217219ebe9e365.zip"},
                {"GZ301VV", "13603-OOLT27-8788c068fdad2a64da9925a5411b30af.zip"},
                {"GZ301VVC", "13793-8DIIWY-72aca4ea0f1db6c991b3e24da5a7907c.zip"},
                {"GZ301ZA", "10139-YXT5A7-521746c1b4991657abc37999a318219e.zip"},
                {"GZ301ZC", "10140-QA59M0-be0cf751aba85c98832f4a0f16456330.zip"},
                {"GZ301ZE", "10141-P1OXLB-318b5c3601e0d5c55d3b4307eba08ab2.zip"},
                {"GZ301ZH", "10142-ELMX6M-f9c50110ca0a763bf4b6d1e9dd9e3afa.zip"},
                {"NR2301L", "14736-3DM4T5-41c1b0b5c2d726da7c66e0c39ea08017.zip"},
                {"RC71L", "17126-IIDPFS-718c077a118724bb0045b35e30d5ac4e.zip"},
                {"RC71X", "15182-KWVC62-c754d9ea39f9160df7e21e4538ce60df.zip"},
            };

            if (profiles.ContainsKey(model))
            {
                return baseUrl + profiles[model];
            }
            else
            {
                return null;
            }
        }

        public static bool ProfileExists()
        {
            (string bios, string model) = AppConfig.GetBiosAndModel();
            return (GetProfileUrl(model) is not null);
        }

        public static async Task InstallProfile()
        {
            ProcessHelper.RunAsAdmin();

            (string bios, string model) = AppConfig.GetBiosAndModel();
            string? profileUrl = GetProfileUrl(model);

            if (profileUrl == null)
            {
                Logger.WriteLine($"No ICC Profile found for {model}");
                return;
            }

            await DownloadAndExtractZip(profileUrl, VisualControl.GetGameVisualPath());
        }

        static async Task DownloadAndExtractZip(string zipUrl, string extractPath)
        {
            using (HttpClient client = new HttpClient())
            {
                try
                {
                    Logger.WriteLine($"Getting: {zipUrl}");
                    byte[] zipData = await client.GetByteArrayAsync(zipUrl);

                    Directory.CreateDirectory(extractPath);

                    string tempZipPath = Path.Combine(extractPath, "temp.zip");
                    await File.WriteAllBytesAsync(tempZipPath, zipData);

                    ZipFile.ExtractToDirectory(tempZipPath, extractPath, true);

                    File.Delete(tempZipPath);
                }
                catch (Exception ex)
                {
                    Logger.WriteLine($"Error: {ex.Message}");
                }
            }
        }

    }
}
    permissions: ﻿namespace GHelper.Display
{

    public class DisplayGammaRamp
    {

        public DisplayGammaRamp(ushort[] red, ushort[] green, ushort[] blue)
        {
            if (red?.Length != GammaRamp.DataPoints)
            {
                throw new ArgumentOutOfRangeException(nameof(red));
            }

            if (green?.Length != GammaRamp.DataPoints)
            {
                throw new ArgumentOutOfRangeException(nameof(green));
            }

            if (blue?.Length != GammaRamp.DataPoints)
            {
                throw new ArgumentOutOfRangeException(nameof(blue));
            }

            Red = red;
            Green = green;
            Blue = blue;
        }

        public DisplayGammaRamp(double brightness = 1, double contrast = 1, double gamma = 1)
            : this(
                CalculateLUT(brightness, contrast, gamma),
                CalculateLUT(brightness, contrast, gamma),
                CalculateLUT(brightness, contrast, gamma)
            )
        {
        }

        public DisplayGammaRamp(
            double redBrightness,
            double redContrast,
            double redGamma,
            double greenBrightness,
            double greenContrast,
            double greenGamma,
            double blueBrightness,
            double blueContrast,
            double blueGamma
        )
            : this(
                CalculateLUT(redBrightness, redContrast, redGamma),
                CalculateLUT(greenBrightness, greenContrast, greenGamma),
                CalculateLUT(blueBrightness, blueContrast, blueGamma)
            )
        {
        }

        internal DisplayGammaRamp(GammaRamp ramp) :
            this(ramp.Red, ramp.Green, ramp.Blue)
        {
        }
        public ushort[] Blue { get; }
        public ushort[] Green { get; }
        public ushort[] Red { get; }
        private static ushort[] CalculateLUT(double brightness, double contrast, double gamma)
        {
            brightness = 0.5 + brightness / 2;
            var result = new ushort[GammaRamp.DataPoints];
            for (var i = 0; i < result.Length; i++)
            {
                result[i] = (ushort)(brightness * ushort.MaxValue * i / (float)(result.Length - 1));
            }
            return result;
        }

        public bool IsOriginal()
        {
            int MaxRed = Red[Red.Length - 1];
            int MaxGreen = Green[Green.Length - 1];
            int MaxBlue = Blue[Blue.Length - 1];
            return (Math.Abs((MaxRed + MaxGreen + MaxBlue) / 3 - ushort.MaxValue) < 256);
        }

        private static ushort[] Brightness(ushort[] data, double brightness)
        {
            var result = new ushort[GammaRamp.DataPoints];
            for (var i = 0; i < result.Length; i++)
            {
                if (brightness < 0.5)
                    result[i] = (ushort)(0.5 * ushort.MaxValue * Math.Pow((float)i/(result.Length - 1),  2 - brightness*2));
                else
                    result[i] = (ushort)(data[i] * brightness);
            }
            return result;
        }

        internal GammaRamp AsBrightnessRamp(double brightness)
        {
            return new GammaRamp(
                Brightness(Red, brightness),
                Brightness(Green, brightness),
                Brightness(Blue, brightness)
            );
        }

        internal GammaRamp AsRamp()
        {
            return new GammaRamp(Red, Green, Blue);
        }


    }
}
      contents: ﻿using System.Runtime.InteropServices;

namespace GHelper.Display
{

    [StructLayout(LayoutKind.Sequential)]
    internal struct GammaRamp
    {
        public const int DataPoints = 256;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = DataPoints)]
        public readonly ushort[] Red;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = DataPoints)]
        public readonly ushort[] Green;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = DataPoints)]
        public readonly ushort[] Blue;

        public GammaRamp(ushort[] red, ushort[] green, ushort[] blue)
        {
            if (red == null)
            {
                throw new ArgumentNullException(nameof(red));
            }

            if (green == null)
            {
                throw new ArgumentNullException(nameof(green));
            }

            if (blue == null)
            {
                throw new ArgumentNullException(nameof(blue));
            }

            if (red.Length != DataPoints)
            {
                throw new ArgumentOutOfRangeException(nameof(red));
            }

            if (green.Length != DataPoints)
            {
                throw new ArgumentOutOfRangeException(nameof(green));
            }

            if (blue.Length != DataPoints)
            {
                throw new ArgumentOutOfRangeException(nameof(blue));
            }

            Red = red;
            Green = green;
            Blue = blue;
        }
    }

}
      security-events: ﻿namespace GHelper.Display
{
    using System;
    using System.Diagnostics;
    using System.Management;

    public static class ScreenBrightness
    {
        public static int Get()
        {
            using var mclass = new ManagementClass("WmiMonitorBrightness")
            {
                Scope = new ManagementScope(@"\\.\root\wmi")
            };
            using var instances = mclass.GetInstances();
            foreach (ManagementObject instance in instances)
            {
                return (byte)instance.GetPropertyValue("CurrentBrightness");
            }
            return 0;
        }

        public static void Set(int brightness)
        {
            using var mclass = new ManagementClass("WmiMonitorBrightnessMethods")
            {
                Scope = new ManagementScope(@"\\.\root\wmi")
            };
            using var instances = mclass.GetInstances();
            var args = new object[] { 1, brightness };
            foreach (ManagementObject instance in instances)
            {
                instance.InvokeMethod("WmiSetBrightness", args);
            }
        }

        public static int Adjust(int delta)
        {
            int brightness = Get();
            Debug.WriteLine(brightness);
            brightness = Math.Min(100, Math.Max(0, brightness + delta));
            Set(brightness);
            return brightness;
        }

    }
}
      actions: ﻿using System.ComponentModel;
using System.Runtime.InteropServices;

namespace GHelper.Display
{
    public class ScreenCCD
    {

        public static bool GetHDRStatus(bool log = true)
        {
            var err = GetDisplayConfigBufferSizes(QDC.QDC_ONLY_ACTIVE_PATHS, out var pathCount, out var modeCount);
            if (err != 0)
                throw new Win32Exception(err);

            var paths = new DISPLAYCONFIG_PATH_INFO[pathCount];
            var modes = new DISPLAYCONFIG_MODE_INFO[modeCount];
            err = QueryDisplayConfig(QDC.QDC_ONLY_ACTIVE_PATHS, ref pathCount, paths, ref modeCount, modes, IntPtr.Zero);
            if (err != 0)
                throw new Win32Exception(err);

            string internalName = AppConfig.GetString("internal_display");

            foreach (var path in paths)
            {
                // get display name
                var info = new DISPLAYCONFIG_TARGET_DEVICE_NAME();
                info.header.type = DISPLAYCONFIG_DEVICE_INFO_TYPE.DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                info.header.size = Marshal.SizeOf<DISPLAYCONFIG_TARGET_DEVICE_NAME>();
                info.header.adapterId = path.targetInfo.adapterId;
                info.header.id = path.targetInfo.id;
                err = DisplayConfigGetDeviceInfo(ref info);
                if (err != 0)
                    throw new Win32Exception(err);

                var colorInfo = new DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO();
                colorInfo.header.type = DISPLAYCONFIG_DEVICE_INFO_TYPE.DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
                colorInfo.header.size = Marshal.SizeOf<DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO>();
                colorInfo.header.adapterId = path.targetInfo.adapterId;
                colorInfo.header.id = path.targetInfo.id;
                err = DisplayConfigGetDeviceInfo(ref colorInfo);
                if (err != 0)
                    throw new Win32Exception(err);


                if (info.outputTechnology == DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY.DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                    info.outputTechnology == DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY.DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                    info.monitorFriendlyDeviceName == internalName)
                {
                    if (log) Logger.WriteLine(info.monitorFriendlyDeviceName + " HDR: " + colorInfo.advancedColorEnabled + " " + colorInfo.bitsPerColorChannel + " " + colorInfo.value + " " + colorInfo.wideColorEnforced);
                    return colorInfo.advancedColorEnabled && (colorInfo.bitsPerColorChannel > 8 || !colorInfo.wideColorEnforced);
                }

            }

            return false;

        }

        private enum DISPLAYCONFIG_DEVICE_INFO_TYPE
        {
            DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME = 1,
            DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME = 2,
            DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_PREFERRED_MODE = 3,
            DISPLAYCONFIG_DEVICE_INFO_GET_ADAPTER_NAME = 4,
            DISPLAYCONFIG_DEVICE_INFO_SET_TARGET_PERSISTENCE = 5,
            DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_BASE_TYPE = 6,
            DISPLAYCONFIG_DEVICE_INFO_GET_SUPPORT_VIRTUAL_RESOLUTION = 7,
            DISPLAYCONFIG_DEVICE_INFO_SET_SUPPORT_VIRTUAL_RESOLUTION = 8,
            DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO = 9,
            DISPLAYCONFIG_DEVICE_INFO_SET_ADVANCED_COLOR_STATE = 10,
            DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL = 11,
        }

        private enum DISPLAYCONFIG_COLOR_ENCODING
        {
            DISPLAYCONFIG_COLOR_ENCODING_RGB = 0,
            DISPLAYCONFIG_COLOR_ENCODING_YCBCR444 = 1,
            DISPLAYCONFIG_COLOR_ENCODING_YCBCR422 = 2,
            DISPLAYCONFIG_COLOR_ENCODING_YCBCR420 = 3,
            DISPLAYCONFIG_COLOR_ENCODING_INTENSITY = 4,
        }

        private enum DISPLAYCONFIG_SCALING
        {
            DISPLAYCONFIG_SCALING_IDENTITY = 1,
            DISPLAYCONFIG_SCALING_CENTERED = 2,
            DISPLAYCONFIG_SCALING_STRETCHED = 3,
            DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX = 4,
            DISPLAYCONFIG_SCALING_CUSTOM = 5,
            DISPLAYCONFIG_SCALING_PREFERRED = 128,
        }

        private enum DISPLAYCONFIG_ROTATION
        {
            DISPLAYCONFIG_ROTATION_IDENTITY = 1,
            DISPLAYCONFIG_ROTATION_ROTATE90 = 2,
            DISPLAYCONFIG_ROTATION_ROTATE180 = 3,
        }

        private enum DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY
        {
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER = -1,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15 = 0,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO = 1,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO = 2,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO = 3,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI = 4,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI = 5,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS = 6,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN = 8,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI = 9,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL = 10,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED = 11,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL = 12,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED = 13,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE = 14,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST = 15,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_WIRED = 16,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_VIRTUAL = 17,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL = unchecked((int)0x80000000),
        }

        private enum DISPLAYCONFIG_TOPOLOGY_ID
        {
            DISPLAYCONFIG_TOPOLOGY_INTERNAL = 0x00000001,
            DISPLAYCONFIG_TOPOLOGY_CLONE = 0x00000002,
            DISPLAYCONFIG_TOPOLOGY_EXTEND = 0x00000004,
            DISPLAYCONFIG_TOPOLOGY_EXTERNAL = 0x00000008,
        }

        private enum DISPLAYCONFIG_PATH
        {
            DISPLAYCONFIG_PATH_ACTIVE = 0x00000001,
            DISPLAYCONFIG_PATH_PREFERRED_UNSCALED = 0x00000004,
            DISPLAYCONFIG_PATH_SUPPORT_VIRTUAL_MODE = 0x00000008,
        }

        private enum DISPLAYCONFIG_SOURCE_FLAGS
        {
            DISPLAYCONFIG_SOURCE_IN_USE = 0x00000001,
        }

        private enum DISPLAYCONFIG_TARGET_FLAGS
        {
            DISPLAYCONFIG_TARGET_IN_USE = 0x00000001,
            DISPLAYCONFIG_TARGET_FORCIBLE = 0x00000002,
            DISPLAYCONFIG_TARGET_FORCED_AVAILABILITY_BOOT = 0x00000004,
            DISPLAYCONFIG_TARGET_FORCED_AVAILABILITY_PATH = 0x00000008,
            DISPLAYCONFIG_TARGET_FORCED_AVAILABILITY_SYSTEM = 0x00000010,
            DISPLAYCONFIG_TARGET_IS_HMD = 0x00000020,
        }

        private enum QDC
        {
            QDC_ALL_PATHS = 0x00000001,
            QDC_ONLY_ACTIVE_PATHS = 0x00000002,
            QDC_DATABASE_CURRENT = 0x00000004,
            QDC_VIRTUAL_MODE_AWARE = 0x00000010,
            QDC_INCLUDE_HMD = 0x00000020,
        }

        private enum DISPLAYCONFIG_SCANLINE_ORDERING
        {
            DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED = 0,
            DISPLAYCONFIG_SCANLINE_ORDERING_PROGRESSIVE = 1,
            DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED = 2,
            DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED_UPPERFIELDFIRST = DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED,
            DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED_LOWERFIELDFIRST = 3,
        }

        private enum DISPLAYCONFIG_PIXELFORMAT
        {
            DISPLAYCONFIG_PIXELFORMAT_8BPP = 1,
            DISPLAYCONFIG_PIXELFORMAT_16BPP = 2,
            DISPLAYCONFIG_PIXELFORMAT_24BPP = 3,
            DISPLAYCONFIG_PIXELFORMAT_32BPP = 4,
            DISPLAYCONFIG_PIXELFORMAT_NONGDI = 5,
        }

        private enum DISPLAYCONFIG_MODE_INFO_TYPE
        {
            DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE = 1,
            DISPLAYCONFIG_MODE_INFO_TYPE_TARGET = 2,
            DISPLAYCONFIG_MODE_INFO_TYPE_DESKTOP_IMAGE = 3,
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_DEVICE_INFO_HEADER
        {
            public DISPLAYCONFIG_DEVICE_INFO_TYPE type;
            public int size;
            public LUID adapterId;
            public uint id;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO
        {
            public DISPLAYCONFIG_DEVICE_INFO_HEADER header;
            public uint value;
            public DISPLAYCONFIG_COLOR_ENCODING colorEncoding;
            public int bitsPerColorChannel;

            public bool advancedColorSupported => (value & 0x1) == 0x1;
            public bool advancedColorEnabled => (value & 0x2) == 0x2;
            public bool wideColorEnforced => (value & 0x4) == 0x4;
            public bool advancedColorForceDisabled => (value & 0x8) == 0x8;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct POINTL
        {
            public int x;
            public int y;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct LUID
        {
            public uint LowPart;
            public int HighPart;

            public long Value => ((long)HighPart << 32) | LowPart;
            public override string ToString() => Value.ToString();
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_SOURCE_MODE
        {
            public uint width;
            public uint height;
            public DISPLAYCONFIG_PIXELFORMAT pixelFormat;
            public POINTL position;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_RATIONAL
        {
            public uint Numerator;
            public uint Denominator;

            public override string ToString() => Numerator + " / " + Denominator;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_2DREGION
        {
            public uint cx;
            public uint cy;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_DESKTOP_IMAGE_INFO
        {
            public POINTL PathSourceSize;
            public RECT DesktopImageRegion;
            public RECT DesktopImageClip;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_VIDEO_SIGNAL_INFO
        {
            public ulong pixelRate;
            public DISPLAYCONFIG_RATIONAL hSyncFreq;
            public DISPLAYCONFIG_RATIONAL vSyncFreq;
            public DISPLAYCONFIG_2DREGION activeSize;
            public DISPLAYCONFIG_2DREGION totalSize;
            public uint videoStandard;
            public DISPLAYCONFIG_SCANLINE_ORDERING scanLineOrdering;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_TARGET_MODE
        {
            public DISPLAYCONFIG_VIDEO_SIGNAL_INFO targetVideoSignalInfo;
        }

        [StructLayout(LayoutKind.Explicit)]
        private struct DISPLAYCONFIG_MODE_INFO_union
        {
            [FieldOffset(0)]
            public DISPLAYCONFIG_TARGET_MODE targetMode;

            [FieldOffset(0)]
            public DISPLAYCONFIG_SOURCE_MODE sourceMode;

            [FieldOffset(0)]
            public DISPLAYCONFIG_DESKTOP_IMAGE_INFO desktopImageInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_PATH_SOURCE_INFO
        {
            public LUID adapterId;
            public uint id;
            public uint modeInfoIdx;
            public DISPLAYCONFIG_SOURCE_FLAGS statusFlags;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_PATH_TARGET_INFO
        {
            public LUID adapterId;
            public uint id;
            public uint modeInfoIdx;
            public DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY outputTechnology;
            public DISPLAYCONFIG_ROTATION rotation;
            public DISPLAYCONFIG_SCALING scaling;
            public DISPLAYCONFIG_RATIONAL refreshRate;
            public DISPLAYCONFIG_SCANLINE_ORDERING scanLineOrdering;
            public bool targetAvailable;
            public DISPLAYCONFIG_TARGET_FLAGS statusFlags;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_PATH_INFO
        {
            public DISPLAYCONFIG_PATH_SOURCE_INFO sourceInfo;
            public DISPLAYCONFIG_PATH_TARGET_INFO targetInfo;
            public DISPLAYCONFIG_PATH flags;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DISPLAYCONFIG_MODE_INFO
        {
            public DISPLAYCONFIG_MODE_INFO_TYPE infoType;
            public uint id;
            public LUID adapterId;
            public DISPLAYCONFIG_MODE_INFO_union info;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        private struct DISPLAYCONFIG_SOURCE_DEVICE_NAME
        {
            public DISPLAYCONFIG_DEVICE_INFO_HEADER header;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string viewGdiDeviceName;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        private struct DISPLAYCONFIG_TARGET_DEVICE_NAME_FLAGS
        {
            public uint value;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        private struct DISPLAYCONFIG_TARGET_DEVICE_NAME
        {
            public DISPLAYCONFIG_DEVICE_INFO_HEADER header;
            public DISPLAYCONFIG_TARGET_DEVICE_NAME_FLAGS flags;
            public DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY outputTechnology;
            public ushort edidManufactureId;
            public ushort edidProductCodeId;
            public uint connectorInstance;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string monitorFriendlyDeviceName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string monitorDevicePat;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct RECT
        {
            public int left;
            public int top;
            public int right;
            public int bottom;
        }

        [DllImport("user32")]
        private static extern int GetDisplayConfigBufferSizes(QDC flags, out int numPathArrayElements, out int numModeInfoArrayElements);

        [DllImport("user32")]
        private static extern int QueryDisplayConfig(QDC flags, ref int numPathArrayElements, [In, Out] DISPLAYCONFIG_PATH_INFO[] pathArray, ref int numModeInfoArrayElements, [In, Out] DISPLAYCONFIG_MODE_INFO[] modeInfoArray, out DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId);

        [DllImport("user32")]
        private static extern int QueryDisplayConfig(QDC flags, ref int numPathArrayElements, [In, Out] DISPLAYCONFIG_PATH_INFO[] pathArray, ref int numModeInfoArrayElements, [In, Out] DISPLAYCONFIG_MODE_INFO[] modeInfoArray, IntPtr currentTopologyId);

        [DllImport("user32")]
        private static extern int DisplayConfigGetDeviceInfo(ref DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO requestPacket);

        [DllImport("user32")]
        private static extern int DisplayConfigGetDeviceInfo(ref DISPLAYCONFIG_SOURCE_DEVICE_NAME requestPacket);

        [DllImport("user32")]
        private static extern int DisplayConfigGetDeviceInfo(ref DISPLAYCONFIG_TARGET_DEVICE_NAME requestPacket);
    }
}
    name: ﻿using System.Diagnostics;

namespace GHelper.Display
{
    public class ScreenControl
    {

        public const int MAX_REFRESH = 1000;


        public void AutoScreen(bool force = false)
        {
            if (force || AppConfig.Is("screen_auto"))
            {
                if (SystemInformation.PowerStatus.PowerLineStatus == PowerLineStatus.Online)
                    SetScreen(MAX_REFRESH, 1);
                else
                    SetScreen(60, 0);
            }
            else
            {
                SetScreen(overdrive: AppConfig.Get("overdrive"));
            }
        }

        public void ToggleScreenRate()
        {
            var laptopScreen = ScreenNative.FindLaptopScreen(true);
            var refreshRate = ScreenNative.GetRefreshRate(laptopScreen);
            if (refreshRate < 0) return;

            ScreenNative.SetRefreshRate(laptopScreen, refreshRate > 60 ? 60 : ScreenNative.GetMaxRefreshRate(laptopScreen));
            InitScreen();
        }


        public void SetScreen(int frequency = -1, int overdrive = -1, int miniled = -1)
        {
            var laptopScreen = ScreenNative.FindLaptopScreen(true);
            var refreshRate = ScreenNative.GetRefreshRate(laptopScreen);

            if (refreshRate < 0) return;

            if (frequency >= MAX_REFRESH)
            {
                frequency = ScreenNative.GetMaxRefreshRate(laptopScreen);
            }

            if (frequency > 0 && frequency != refreshRate)
            {
                ScreenNative.SetRefreshRate(laptopScreen, frequency);
            }

            if (Program.acpi.IsOverdriveSupported() && overdrive >= 0)
            {
                if (AppConfig.IsNoOverdrive()) overdrive = 0;
                if (overdrive != Program.acpi.DeviceGet(AsusACPI.ScreenOverdrive))
                {
                    Program.acpi.DeviceSet(AsusACPI.ScreenOverdrive, overdrive, "ScreenOverdrive");
                }
            }

            SetMiniled(miniled);

            InitScreen();
        }

        public void SetMiniled(int miniled = -1)
        {
            if (miniled >= 0)
            {
                if (Program.acpi.DeviceGet(AsusACPI.ScreenMiniled1) >= 0)
                    Program.acpi.DeviceSet(AsusACPI.ScreenMiniled1, miniled, "Miniled1");
                else
                {
                    Program.acpi.DeviceSet(AsusACPI.ScreenMiniled2, miniled, "Miniled2");
                    Thread.Sleep(100);
                }
            }
        }

        public void InitMiniled()
        {
            if (AppConfig.IsForceMiniled())
                SetMiniled(AppConfig.Get("miniled"));
        }

        public void ToogleFHD()
        {
            int fhd = Program.acpi.DeviceGet(AsusACPI.ScreenFHD);
            Logger.WriteLine($"FHD Toggle: {fhd}");

            DialogResult dialogResult = MessageBox.Show("Changing display mode requires reboot", Properties.Strings.AlertUltimateTitle, MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                Program.acpi.DeviceSet(AsusACPI.ScreenFHD, (fhd == 1) ? 0 : 1, "FHD");
                Process.Start("shutdown", "/r /t 1");
            }
        }

        public string ToogleMiniled()
        {
            int miniled1 = Program.acpi.DeviceGet(AsusACPI.ScreenMiniled1);
            int miniled2 = Program.acpi.DeviceGet(AsusACPI.ScreenMiniled2);

            Logger.WriteLine($"MiniledToggle: {miniled1} {miniled2}");

            int miniled;
            string name;

            if (miniled1 >= 0)
            {
                switch (miniled1)
                {
                    case 1: 
                        miniled = 0;
                        name = Properties.Strings.OneZone;
                        break;
                    default:
                        miniled = 1;
                        name = Properties.Strings.Multizone;
                        break;
                }
            }
            else
            {
                switch (miniled2)
                {
                    case 1: 
                        miniled = 2;
                        name = Properties.Strings.OneZone;
                        break;
                    case 2: 
                        miniled = 0;
                        name = Properties.Strings.Multizone;
                        break;
                    default: 
                        miniled = 1;
                        name = Properties.Strings.MultizoneStrong;
                        break;
                }
            }

            AppConfig.Set("miniled", miniled);
            SetScreen(miniled: miniled);

            return name;
        }

        public void InitScreen()
        {
            var laptopScreen = ScreenNative.FindLaptopScreen();
            int frequency = ScreenNative.GetRefreshRate(laptopScreen);
            int maxFrequency = ScreenNative.GetMaxRefreshRate(laptopScreen);

            bool screenAuto = AppConfig.Is("screen_auto");
            bool overdriveSetting = Program.acpi.IsOverdriveSupported() && !AppConfig.IsNoOverdrive();

            int overdrive = AppConfig.IsNoOverdrive() ? 0 : Program.acpi.DeviceGet(AsusACPI.ScreenOverdrive);

            int miniled1 = Program.acpi.DeviceGet(AsusACPI.ScreenMiniled1);
            int miniled2 = Program.acpi.DeviceGet(AsusACPI.ScreenMiniled2);

            int miniled = (miniled1 >= 0) ? miniled1 : miniled2;
            bool hdr = false;

            if (miniled >= 0)
            {
                Logger.WriteLine($"Miniled: {miniled1} {miniled2}");
                AppConfig.Set("miniled", miniled);
            }

            try
            {
                hdr = ScreenCCD.GetHDRStatus();
            } catch (Exception ex)
            {
                Logger.WriteLine(ex.Message);
            }

            bool screenEnabled = (frequency >= 0);

            int fhd = -1;
            if (AppConfig.IsDUO())
            {
                fhd = Program.acpi.DeviceGet(AsusACPI.ScreenFHD);
            }

            AppConfig.Set("frequency", frequency);
            AppConfig.Set("overdrive", overdrive);

            Program.settingsForm.Invoke(delegate
            {
                Program.settingsForm.VisualiseScreen(
                    screenEnabled: screenEnabled,
                    screenAuto: screenAuto,
                    frequency: frequency,
                    maxFrequency: maxFrequency,
                    overdrive: overdrive,
                    overdriveSetting: overdriveSetting,
                    miniled1: miniled1,
                    miniled2: miniled2,
                    hdr: hdr,
                    fhd: fhd
                );
            });

        }
    }
}
    runs-on: ﻿using GHelper.Helpers;
using NvAPIWrapper.Display;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace GHelper.Display
{
    public static class ScreenInterrogatory
    {
        public const int ERROR_SUCCESS = 0;

        #region enums

        public enum QUERY_DEVICE_CONFIG_FLAGS : uint
        {
            QDC_ALL_PATHS = 0x00000001,
            QDC_ONLY_ACTIVE_PATHS = 0x00000002,
            QDC_DATABASE_CURRENT = 0x00000004
        }

        public enum DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY : uint
        {
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_OTHER = 0xFFFFFFFF,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15 = 0,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SVIDEO = 1,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPOSITE_VIDEO = 2,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_COMPONENT_VIDEO = 3,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI = 4,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI = 5,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS = 6,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_D_JPN = 8,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDI = 9,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL = 10,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED = 11,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EXTERNAL = 12,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED = 13,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_SDTVDONGLE = 14,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST = 15,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL = 0x80000000,
            DISPLAYCONFIG_OUTPUT_TECHNOLOGY_FORCE_UINT32 = 0xFFFFFFFF
        }

        public enum DISPLAYCONFIG_SCANLINE_ORDERING : uint
        {
            DISPLAYCONFIG_SCANLINE_ORDERING_UNSPECIFIED = 0,
            DISPLAYCONFIG_SCANLINE_ORDERING_PROGRESSIVE = 1,
            DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED = 2,
            DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED_UPPERFIELDFIRST = DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED,
            DISPLAYCONFIG_SCANLINE_ORDERING_INTERLACED_LOWERFIELDFIRST = 3,
            DISPLAYCONFIG_SCANLINE_ORDERING_FORCE_UINT32 = 0xFFFFFFFF
        }

        public enum DISPLAYCONFIG_ROTATION : uint
        {
            DISPLAYCONFIG_ROTATION_IDENTITY = 1,
            DISPLAYCONFIG_ROTATION_ROTATE90 = 2,
            DISPLAYCONFIG_ROTATION_ROTATE180 = 3,
            DISPLAYCONFIG_ROTATION_ROTATE270 = 4,
            DISPLAYCONFIG_ROTATION_FORCE_UINT32 = 0xFFFFFFFF
        }

        public enum DISPLAYCONFIG_SCALING : uint
        {
            DISPLAYCONFIG_SCALING_IDENTITY = 1,
            DISPLAYCONFIG_SCALING_CENTERED = 2,
            DISPLAYCONFIG_SCALING_STRETCHED = 3,
            DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX = 4,
            DISPLAYCONFIG_SCALING_CUSTOM = 5,
            DISPLAYCONFIG_SCALING_PREFERRED = 128,
            DISPLAYCONFIG_SCALING_FORCE_UINT32 = 0xFFFFFFFF
        }

        public enum DISPLAYCONFIG_PIXELFORMAT : uint
        {
            DISPLAYCONFIG_PIXELFORMAT_8BPP = 1,
            DISPLAYCONFIG_PIXELFORMAT_16BPP = 2,
            DISPLAYCONFIG_PIXELFORMAT_24BPP = 3,
            DISPLAYCONFIG_PIXELFORMAT_32BPP = 4,
            DISPLAYCONFIG_PIXELFORMAT_NONGDI = 5,
            DISPLAYCONFIG_PIXELFORMAT_FORCE_UINT32 = 0xffffffff
        }

        public enum DISPLAYCONFIG_MODE_INFO_TYPE : uint
        {
            DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE = 1,
            DISPLAYCONFIG_MODE_INFO_TYPE_TARGET = 2,
            DISPLAYCONFIG_MODE_INFO_TYPE_FORCE_UINT32 = 0xFFFFFFFF
        }

        public enum DISPLAYCONFIG_DEVICE_INFO_TYPE : uint
        {
            DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME = 1,
            DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME = 2,
            DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_PREFERRED_MODE = 3,
            DISPLAYCONFIG_DEVICE_INFO_GET_ADAPTER_NAME = 4,
            DISPLAYCONFIG_DEVICE_INFO_SET_TARGET_PERSISTENCE = 5,
            DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_BASE_TYPE = 6,
            DISPLAYCONFIG_DEVICE_INFO_FORCE_UINT32 = 0xFFFFFFFF
        }

        #endregion

        #region structs

        [StructLayout(LayoutKind.Sequential)]
        public struct LUID
        {
            public uint LowPart;
            public int HighPart;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_PATH_SOURCE_INFO
        {
            public LUID adapterId;
            public uint id;
            public uint modeInfoIdx;
            public uint statusFlags;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_PATH_TARGET_INFO
        {
            public LUID adapterId;
            public uint id;
            public uint modeInfoIdx;
            private DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY outputTechnology;
            private DISPLAYCONFIG_ROTATION rotation;
            private DISPLAYCONFIG_SCALING scaling;
            private DISPLAYCONFIG_RATIONAL refreshRate;
            private DISPLAYCONFIG_SCANLINE_ORDERING scanLineOrdering;
            public bool targetAvailable;
            public uint statusFlags;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_RATIONAL
        {
            public uint Numerator;
            public uint Denominator;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_PATH_INFO
        {
            public DISPLAYCONFIG_PATH_SOURCE_INFO sourceInfo;
            public DISPLAYCONFIG_PATH_TARGET_INFO targetInfo;
            public uint flags;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_2DREGION
        {
            public uint cx;
            public uint cy;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_VIDEO_SIGNAL_INFO
        {
            public ulong pixelRate;
            public DISPLAYCONFIG_RATIONAL hSyncFreq;
            public DISPLAYCONFIG_RATIONAL vSyncFreq;
            public DISPLAYCONFIG_2DREGION activeSize;
            public DISPLAYCONFIG_2DREGION totalSize;
            public uint videoStandard;
            public DISPLAYCONFIG_SCANLINE_ORDERING scanLineOrdering;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_TARGET_MODE
        {
            public DISPLAYCONFIG_VIDEO_SIGNAL_INFO targetVideoSignalInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct POINTL
        {
            private int x;
            private int y;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_SOURCE_MODE
        {
            public uint width;
            public uint height;
            public DISPLAYCONFIG_PIXELFORMAT pixelFormat;
            public POINTL position;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct DISPLAYCONFIG_MODE_INFO_UNION
        {
            [FieldOffset(0)]
            public DISPLAYCONFIG_TARGET_MODE targetMode;

            [FieldOffset(0)]
            public DISPLAYCONFIG_SOURCE_MODE sourceMode;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_MODE_INFO
        {
            public DISPLAYCONFIG_MODE_INFO_TYPE infoType;
            public uint id;
            public LUID adapterId;
            public DISPLAYCONFIG_MODE_INFO_UNION modeInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_TARGET_DEVICE_NAME_FLAGS
        {
            public uint value;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct DISPLAYCONFIG_DEVICE_INFO_HEADER
        {
            public DISPLAYCONFIG_DEVICE_INFO_TYPE type;
            public uint size;
            public LUID adapterId;
            public uint id;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct DISPLAYCONFIG_TARGET_DEVICE_NAME
        {
            public DISPLAYCONFIG_DEVICE_INFO_HEADER header;
            public DISPLAYCONFIG_TARGET_DEVICE_NAME_FLAGS flags;
            public DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY outputTechnology;
            public ushort edidManufactureId;
            public ushort edidProductCodeId;
            public uint connectorInstance;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string monitorFriendlyDeviceName;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string monitorDevicePath;
        }

        #endregion

        #region DLL-Imports

        [DllImport("user32.dll")]
        public static extern int GetDisplayConfigBufferSizes(
            QUERY_DEVICE_CONFIG_FLAGS flags, out uint numPathArrayElements, out uint numModeInfoArrayElements);

        [DllImport("user32.dll")]
        public static extern int QueryDisplayConfig(
            QUERY_DEVICE_CONFIG_FLAGS flags,
            ref uint numPathArrayElements, [Out] DISPLAYCONFIG_PATH_INFO[] PathInfoArray,
            ref uint numModeInfoArrayElements, [Out] DISPLAYCONFIG_MODE_INFO[] ModeInfoArray,
            nint currentTopologyId
            );

        [DllImport("user32.dll")]
        public static extern int DisplayConfigGetDeviceInfo(ref DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName);

        #endregion


        [Flags]
        public enum DisplayDeviceStates : int
        {
            ATTACHED_TO_DESKTOP = 0x01,
            PRIMARY_DEVICE = 0x04,
            MIRRORING_DRIVER = 0x08,
            VGA_COMPATIBLE = 0x10,
            REMOVABLE = 0x20,
            DISCONNECTED = 0x2000000,
            REMOTE = 0x4000000,
            MODESPRUNED = 0x8000000
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct DISPLAY_DEVICE
        {
            public int cb;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string DeviceName;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string DeviceString;

            public DisplayDeviceStates StateFlags;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string DeviceID;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string DeviceKey;
        }


        [DllImport(nameof(User32), CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern bool EnumDisplayDevices(
            string? lpDevice,
            uint iDevNum,
            ref DISPLAY_DEVICE lpDisplayDevice,
            uint dwFlags);


        private static DISPLAYCONFIG_TARGET_DEVICE_NAME DeviceName(LUID adapterId, uint targetId)
        {
            var deviceName = new DISPLAYCONFIG_TARGET_DEVICE_NAME
            {
                header =
                {
                    size = (uint)Marshal.SizeOf(typeof (DISPLAYCONFIG_TARGET_DEVICE_NAME)),
                    adapterId = adapterId,
                    id = targetId,
                    type = DISPLAYCONFIG_DEVICE_INFO_TYPE.DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME
                }
            };
            var error = DisplayConfigGetDeviceInfo(ref deviceName);
            if (error != ERROR_SUCCESS)
                throw new Win32Exception(error);

            return deviceName;
        }

        public static IEnumerable<DISPLAYCONFIG_TARGET_DEVICE_NAME> GetAllDevices()
        {
            uint pathCount, modeCount;
            var error = GetDisplayConfigBufferSizes(QUERY_DEVICE_CONFIG_FLAGS.QDC_ONLY_ACTIVE_PATHS, out pathCount, out modeCount);
            if (error != ERROR_SUCCESS)
                throw new Win32Exception(error);

            var displayPaths = new DISPLAYCONFIG_PATH_INFO[pathCount];
            var displayModes = new DISPLAYCONFIG_MODE_INFO[modeCount];
            error = QueryDisplayConfig(QUERY_DEVICE_CONFIG_FLAGS.QDC_ONLY_ACTIVE_PATHS,
                ref pathCount, displayPaths, ref modeCount, displayModes, nint.Zero);
            if (error != ERROR_SUCCESS)
                throw new Win32Exception(error);

            for (var i = 0; i < modeCount; i++)
                if (displayModes[i].infoType == DISPLAYCONFIG_MODE_INFO_TYPE.DISPLAYCONFIG_MODE_INFO_TYPE_TARGET)
                {
                    DISPLAYCONFIG_TARGET_DEVICE_NAME? displayName = null;
                    try
                    {
                        displayName = DeviceName(displayModes[i].adapterId, displayModes[i].id);

                    } catch (Exception e)
                    {
                        Logger.WriteLine(e.Message);
                    }

                    if (displayName is not null) yield return (DISPLAYCONFIG_TARGET_DEVICE_NAME)displayName;
                }

        }


        public static IEnumerable<DISPLAY_DEVICE> GetDisplayDevices()
        {

            var displayAdapter = new DISPLAY_DEVICE();
            displayAdapter.cb = Marshal.SizeOf<DISPLAY_DEVICE>();

            var displayAdapterNumber = default(uint);
            while (EnumDisplayDevices(null, displayAdapterNumber, ref displayAdapter, 1))
            {
                var displayMonitor = new DISPLAY_DEVICE();
                displayMonitor.cb = Marshal.SizeOf<DISPLAY_DEVICE>();

                var displayMonitorNumber = default(uint);
                while (EnumDisplayDevices(displayAdapter.DeviceName, displayMonitorNumber, ref displayMonitor, 1))
                {
                    var isAttached = (displayMonitor.StateFlags & DisplayDeviceStates.ATTACHED_TO_DESKTOP) == DisplayDeviceStates.ATTACHED_TO_DESKTOP;
                    var isMirroring = (displayMonitor.StateFlags & DisplayDeviceStates.MIRRORING_DRIVER) == DisplayDeviceStates.MIRRORING_DRIVER;
                    if (isAttached && !isMirroring) yield return displayMonitor;
                    displayMonitorNumber++;

                }

                displayAdapterNumber++;
            }
        }


    }

}

    steps: ﻿using System.Collections;
using System.Runtime.InteropServices;
using static GHelper.Display.ScreenInterrogatory;

namespace GHelper.Display
{

    class DeviceComparer : IComparer
    {
        public int Compare(object x, object y)
        {
            uint displayX = ((DISPLAYCONFIG_TARGET_DEVICE_NAME)x).connectorInstance;
            uint displayY = ((DISPLAYCONFIG_TARGET_DEVICE_NAME)y).connectorInstance;

            if (displayX > displayY)
                return 1;
            if (displayX < displayY)
                return -1;
            else
                return 0;
        }
    }

    class ScreenComparer : IComparer
    {
        public int Compare(object x, object y)
        {
            int displayX = Int32.Parse(((Screen)x).DeviceName.Replace(@"\\.\DISPLAY", ""));
            int displayY = Int32.Parse(((Screen)y).DeviceName.Replace(@"\\.\DISPLAY", ""));
            return (new CaseInsensitiveComparer()).Compare(displayX, displayY);
        }
    }
    internal class ScreenNative
    {

        [DllImport("gdi32", CharSet = CharSet.Unicode)]
        internal static extern IntPtr CreateDC(string driver, string device, string port, IntPtr deviceMode);

        [DllImport("gdi32")]
        internal static extern bool SetDeviceGammaRamp(IntPtr dcHandle, ref GammaRamp ramp);

        [DllImport("gdi32")]
        internal static extern bool GetDeviceGammaRamp(IntPtr dcHandle, ref GammaRamp ramp);

        [DllImport("gdi32", CharSet = CharSet.Unicode)]
        internal static extern bool SetICMProfileW(IntPtr dcHandle, string lpFileName);

        [DllImport("gdi32", CharSet = CharSet.Unicode)]
        internal static extern bool SetICMMode(IntPtr dcHandle, int mode);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct DEVMODE
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string dmDeviceName;

            public short dmSpecVersion;
            public short dmDriverVersion;
            public short dmSize;
            public short dmDriverExtra;
            public int dmFields;
            public int dmPositionX;
            public int dmPositionY;
            public int dmDisplayOrientation;
            public int dmDisplayFixedOutput;
            public short dmColor;
            public short dmDuplex;
            public short dmYResolution;
            public short dmTTOption;
            public short dmCollate;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string dmFormName;

            public short dmLogPixels;
            public short dmBitsPerPel;
            public int dmPelsWidth;
            public int dmPelsHeight;
            public int dmDisplayFlags;
            public int dmDisplayFrequency;
            public int dmICMMethod;
            public int dmICMIntent;
            public int dmMediaType;
            public int dmDitherType;
            public int dmReserved1;
            public int dmReserved2;
            public int dmPanningWidth;
            public int dmPanningHeight;
        };

        [Flags()]
        public enum DisplaySettingsFlags : int
        {
            CDS_UPDATEREGISTRY = 1,
            CDS_TEST = 2,
            CDS_FULLSCREEN = 4,
            CDS_GLOBAL = 8,
            CDS_SET_PRIMARY = 0x10,
            CDS_RESET = 0x40000000,
            CDS_NORESET = 0x10000000
        }

        // PInvoke declaration for EnumDisplaySettings Win32 API
        [DllImport("user32.dll")]
        public static extern int EnumDisplaySettingsEx(
             string lpszDeviceName,
             int iModeNum,
             ref DEVMODE lpDevMode);

        // PInvoke declaration for ChangeDisplaySettings Win32 API
        [DllImport("user32.dll")]
        public static extern int ChangeDisplaySettingsEx(
                string lpszDeviceName, ref DEVMODE lpDevMode, IntPtr hwnd,
                DisplaySettingsFlags dwflags, IntPtr lParam);

        public static DEVMODE CreateDevmode()
        {
            DEVMODE dm = new DEVMODE();
            dm.dmDeviceName = new String(new char[32]);
            dm.dmFormName = new String(new char[32]);
            dm.dmSize = (short)Marshal.SizeOf(dm);
            return dm;
        }

        public enum COLORPROFILETYPE
        {
            CPT_ICC,
            CPT_DMP,
            CPT_CAMP,
            CPT_GMMP
        }
        public enum COLORPROFILESUBTYPE
        {
            CPST_PERCEPTUAL,
            CPST_RELATIVE_COLORIMETRIC,
            CPST_SATURATION,
            CPST_ABSOLUTE_COLORIMETRIC,
            CPST_NONE,
            CPST_RGB_WORKING_SPACE,
            CPST_CUSTOM_WORKING_SPACE,
            CPST_STANDARD_DISPLAY_COLOR_MODE,
            CPST_EXTENDED_DISPLAY_COLOR_MODE
        }
        public enum WCS_PROFILE_MANAGEMENT_SCOPE
        {
            WCS_PROFILE_MANAGEMENT_SCOPE_SYSTEM_WIDE,
            WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER
        }

        [DllImport("mscms.dll", CharSet = CharSet.Unicode)]
        public static extern bool WcsSetDefaultColorProfile(
            WCS_PROFILE_MANAGEMENT_SCOPE scope,
            string pDeviceName,
            COLORPROFILETYPE cptColorProfileType,
            COLORPROFILESUBTYPE cpstColorProfileSubType,
            uint dwProfileID,
            string pProfileName
        );


        public const int ENUM_CURRENT_SETTINGS = -1;
        public const string defaultDevice = @"\\.\DISPLAY1";


        public static string? FindInternalName(bool log = false)
        {
            try
            {
                var devicesList = GetAllDevices();
                var devices = devicesList.ToArray();
                string internalName = AppConfig.GetString("internal_display");

                foreach (var device in devices)
                {
                    if (device.outputTechnology == DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY.DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                        device.outputTechnology == DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY.DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                        device.monitorFriendlyDeviceName == internalName)
                    {
                        if (log) Logger.WriteLine(device.monitorDevicePath + " " + device.outputTechnology);

                        AppConfig.Set("internal_display", device.monitorFriendlyDeviceName);
                        var names = device.monitorDevicePath.Split("#");

                        if (names.Length > 1) return names[1];
                        else return "";
                    }
                }
            }
            catch (Exception ex)
            {
                Logger.WriteLine(ex.ToString());
            }

            return null;
        }

        static string ExtractDisplay(string input)
        {
            int index = input.IndexOf('\\', 4); // Start searching from index 4 to skip ""

            if (index != -1)
            {
                string extracted = input.Substring(0, index);
                return extracted;
            }

            return input;
        }

        public static string? FindLaptopScreen(bool log = false)
        {
            string? laptopScreen = null;
            string? internalName = FindInternalName(log);

            if (internalName == null)
            {
                Logger.WriteLine("Internal screen off");
                return null;
            }

            try
            {
                var displays = GetDisplayDevices().ToArray();
                foreach (var display in displays)
                {
                    if (log) Logger.WriteLine(display.DeviceID + " " + display.DeviceName);
                    if (display.DeviceID.Contains(internalName))
                    {
                        laptopScreen = ExtractDisplay(display.DeviceName);
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Logger.WriteLine(ex.ToString());
            }

            if (laptopScreen is null)
            {
                Logger.WriteLine("Default internal screen");
                laptopScreen = Screen.PrimaryScreen.DeviceName;
            }

            return laptopScreen;
        }


        public static int GetMaxRefreshRate(string? laptopScreen)
        {

            if (laptopScreen is null) return -1;

            DEVMODE dm = CreateDevmode();
            int frequency = -1;

            int i = 0;
            while (0 != EnumDisplaySettingsEx(laptopScreen, i, ref dm))
            {
                if (dm.dmDisplayFrequency > frequency) frequency = dm.dmDisplayFrequency;
                i++;
            }

            if (frequency > 0) AppConfig.Set("screen_max", frequency);
            else frequency = AppConfig.Get("screen_max");

            return frequency;

        }

        public static int GetRefreshRate(string? laptopScreen)
        {

            if (laptopScreen is null) return -1;

            DEVMODE dm = CreateDevmode();
            int frequency = -1;

            if (0 != EnumDisplaySettingsEx(laptopScreen, ENUM_CURRENT_SETTINGS, ref dm))
            {
                frequency = dm.dmDisplayFrequency;
            }

            return frequency;
        }

        public static int SetRefreshRate(string laptopScreen, int frequency = 120)
        {
            DEVMODE dm = CreateDevmode();

            if (0 != EnumDisplaySettingsEx(laptopScreen, ENUM_CURRENT_SETTINGS, ref dm))
            {
                dm.dmDisplayFrequency = frequency;
                int iRet = ChangeDisplaySettingsEx(laptopScreen, ref dm, IntPtr.Zero, DisplaySettingsFlags.CDS_UPDATEREGISTRY, IntPtr.Zero);
                Logger.WriteLine("Screen = " + frequency.ToString() + "Hz : " + (iRet == 0 ? "OK" : iRet));
                return iRet;
            }

            return 0;

        }
    }
}
      - name: ﻿using GHelper.Helpers;
using System.Management;

namespace GHelper.Display
{
    public enum SplendidGamut : int
    {
        VivoNative = 0,
        VivoSRGB = 1,
        VivoDCIP3 = 3,
        ViviDisplayP3 = 4,
        Native = 50,
        sRGB = 51,
        DCIP3 = 53,
        DisplayP3 = 54
    }

    public enum SplendidCommand : int
    {
        None = -1,

        VivoNormal = 1,
        VivoVivid = 2,
        VivoManual = 6,
        VivoEycare = 7,

        Init = 10,
        DimmingVivo = 9,
        DimmingVisual = 19,
        GamutMode = 200,

        Default = 11,
        Racing = 21,
        Scenery = 22,
        RTS = 23,
        FPS = 24,
        Cinema = 25,
        Vivid = 13,
        Eyecare = 17,
    }
    public static class VisualControl
    {
        public static DisplayGammaRamp? gammaRamp;

        private static int _brightness = 100;
        private static bool _init = true;
        private static string? _splendidPath = null;

        private static System.Timers.Timer brightnessTimer = new System.Timers.Timer(200);

        public const int DefaultColorTemp = 50;

        public static bool forceVisual = false;
        public static bool skipGamut = false;

        static VisualControl()
        {
            brightnessTimer.Elapsed += BrightnessTimerTimer_Elapsed;
        }

        public static string GetGameVisualPath()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\ASUS\\GameVisual";
        }

        public static string GetVivobookPath()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\ASUS\\ASUS System Control Interface\\ASUSOptimization\\Splendid";
        }

        public static SplendidGamut GetDefaultGamut()
        {
            return AppConfig.IsVivoZenbook() ? SplendidGamut.VivoNative : SplendidGamut.Native;
        }

        public static Dictionary<SplendidGamut, string> GetGamutModes()
        {

            bool isVivo = AppConfig.IsVivoZenbook();

            Dictionary<SplendidGamut, string> _modes = new Dictionary<SplendidGamut, string>();

            string iccPath = isVivo ? GetVivobookPath() : GetGameVisualPath();

            if (!Directory.Exists(iccPath))
            {
                Logger.WriteLine(iccPath + " doesn't exit");
                return _modes;
            }

            try
            {
                DirectoryInfo d = new DirectoryInfo(iccPath);
                FileInfo[] icms = d.GetFiles("*.icm");
                if (icms.Length == 0) return _modes;

                _modes.Add(isVivo ? SplendidGamut.VivoNative : SplendidGamut.Native, "Gamut: Native");
                foreach (FileInfo icm in icms)
                {
                    //Logger.WriteLine(icm.FullName);

                    if (icm.Name.Contains("sRGB"))
                    {
                        try
                        {
                            _modes.Add(isVivo ? SplendidGamut.VivoSRGB : SplendidGamut.sRGB, "Gamut: sRGB");
                            Logger.WriteLine(icm.FullName + " sRGB");
                        }
                        catch 
                        {
                        }
                    }

                    if (icm.Name.Contains("DCIP3"))
                    {
                        try
                        {
                            _modes.Add(isVivo ? SplendidGamut.VivoDCIP3 : SplendidGamut.DCIP3, "Gamut: DCIP3");
                            Logger.WriteLine(icm.FullName + " DCIP3");
                        }
                        catch
                        {
                        }
                    }

                    if (icm.Name.Contains("DisplayP3"))
                    {
                        try
                        {
                            _modes.Add(isVivo ? SplendidGamut.ViviDisplayP3 : SplendidGamut.DisplayP3, "Gamut: DisplayP3");
                            Logger.WriteLine(icm.FullName + " DisplayP3");
                        }
                        catch
                        {
                        }
                    }
                }
                return _modes;
            }
            catch (Exception ex)
            {
                //Logger.WriteLine(ex.Message);
                Logger.WriteLine(ex.ToString());
                return _modes;
            }

        }

        public static SplendidCommand GetDefaultVisualMode()
        {
            return AppConfig.IsVivoZenbook() ? SplendidCommand.VivoNormal : SplendidCommand.Default;
        }

        public static Dictionary<SplendidCommand, string> GetVisualModes()
        {

            if (AppConfig.IsVivoZenbook())
            {
                return new Dictionary<SplendidCommand, string>
                {
                    { SplendidCommand.VivoNormal, "Default" },
                    { SplendidCommand.VivoVivid, "Vivid" },
                    { SplendidCommand.VivoManual, "Manual" },
                    { SplendidCommand.VivoEycare, "Eyecare" },
                };
            }

            return new Dictionary<SplendidCommand, string>
            {
                { SplendidCommand.Default, "Default"},
                { SplendidCommand.Racing, "Racing"},
                { SplendidCommand.Scenery, "Scenery"},
                { SplendidCommand.RTS, "RTS/RPG"},
                { SplendidCommand.FPS, "FPS"},
                { SplendidCommand.Cinema, "Cinema"},
                { SplendidCommand.Vivid, "Vivid" },
                { SplendidCommand.Eyecare, "Eyecare"}
            };
        }

        public static Dictionary<int, string> GetTemperatures()
        {
            return new Dictionary<int, string>
            {
                { 0, "Warmest"},
                { 15, "Warmer"},
                { 30, "Warm"},
                { 50, "Neutral"},
                { 70, "Cold"},
                { 85, "Colder"},
                { 100, "Coldest"},
            };
        }

        public static Dictionary<int, string> GetEyeCares()
        {
            return new Dictionary<int, string>
            {
                { 0, "0"},
                { 1, "1"},
                { 2, "2"},
                { 3, "3"},
                { 4, "4"},
            };
        }

        public static void SetGamut(int mode = -1)
        {
            if (skipGamut) return;
            if (mode < 0) mode = (int)GetDefaultGamut();

            AppConfig.Set("gamut", mode);

            if (RunSplendid(SplendidCommand.GamutMode, 0, mode)) return;

            if (_init)
            {
                _init = false;
                RunSplendid(SplendidCommand.Init);
                RunSplendid(SplendidCommand.GamutMode, 0, mode);
            }
        }

        public static void SetVisual(SplendidCommand mode = SplendidCommand.Default, int whiteBalance = DefaultColorTemp, bool init = false)
        {
            if (mode == SplendidCommand.None) return;
            if (mode == SplendidCommand.Default && init) return; // Skip default setting on init

            if (!forceVisual && ScreenCCD.GetHDRStatus(true)) return;
            if (!forceVisual && ScreenNative.GetRefreshRate(ScreenNative.FindLaptopScreen(true)) < 0) return;

            AppConfig.Set("visual", (int)mode);
            AppConfig.Set("color_temp", whiteBalance);

            if (whiteBalance != DefaultColorTemp && !init) ProcessHelper.RunAsAdmin();

            int? balance;

            switch (mode)
            {
                case SplendidCommand.Eyecare:
                    balance = 2;
                    break;
                case SplendidCommand.VivoNormal:
                case SplendidCommand.VivoVivid:
                    balance = null;
                    break;
                case SplendidCommand.VivoEycare:
                    balance = Math.Abs(whiteBalance - 50) * 4 / 50;
                    break;
                default:
                    balance = whiteBalance;
                    break;
            }

            if (RunSplendid(mode, 0, balance)) return;

            if (_init)
            {
                _init = false;
                RunSplendid(SplendidCommand.Init);
                RunSplendid(mode, 0, balance);
            }
        }

        private static string GetSplendidPath()
        {
            if (_splendidPath == null)
            {
                try
                {
                    using (var searcher = new ManagementObjectSearcher(@"Select * from Win32_SystemDriver WHERE Name='ATKWMIACPIIO'"))
                    {
                        foreach (var driver in searcher.Get())
                        {
                            string path = driver["PathName"].ToString();
                            _splendidPath = Path.GetDirectoryName(path) + "\\AsusSplendid.exe";
                            break;
                        }
                    }
                }
                catch (Exception ex)
                {
                    Logger.WriteLine(ex.Message);
                }
            }

            return _splendidPath;
        }

        private static bool RunSplendid(SplendidCommand command, int? param1 = null, int? param2 = null)
        {
            var splendid = GetSplendidPath();
            bool isVivo = AppConfig.IsVivoZenbook();
            bool isSplenddid = File.Exists(splendid);

            if (isSplenddid)
            {
                if (command == SplendidCommand.DimmingVisual && isVivo) command = SplendidCommand.DimmingVivo;
                var result = ProcessHelper.RunCMD(splendid, (int)command + " " + param1 + " " + param2);
                if (result.Contains("file not exist") || (result.Length == 0 && !isVivo)) return false;
            }

            return true;
        }

        private static void BrightnessTimerTimer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            brightnessTimer.Stop();


            if (RunSplendid(SplendidCommand.DimmingVisual, 0, (int)(40 + _brightness * 0.6))) return;

            if (_init)
            {
                _init = false;
                RunSplendid(SplendidCommand.Init);
                RunSplendid(SplendidCommand.Init, 4);
                if (RunSplendid(SplendidCommand.DimmingVisual, 0, (int)(40 + _brightness * 0.6))) return;
            }

            // GammaRamp Fallback
            SetGamma(_brightness);
        }

        public static int SetBrightness(int brightness = -1, int delta = 0)
        {
            if (!AppConfig.IsOLED()) return -1;

            if (brightness < 0) brightness = AppConfig.Get("brightness", 100);

            _brightness = Math.Max(0, Math.Min(100, brightness + delta));
            AppConfig.Set("brightness", _brightness);

            brightnessTimer.Start();

            Program.settingsForm.VisualiseBrightness();
            //if (brightness < 100) ResetGamut();

            return _brightness;
        }

        public static void ResetGamut()
        {
            int defaultGamut = (int)GetDefaultGamut();

            if (AppConfig.Get("gamut") != defaultGamut)
            {
                skipGamut = true;
                AppConfig.Set("gamut", defaultGamut);
                Program.settingsForm.VisualiseGamut();
                skipGamut = false;
            }
        }


        public static void SetGamma(int brightness = 100)
        {
            var bright = Math.Round((float)brightness / 200 + 0.5, 2);

            var screenName = ScreenNative.FindLaptopScreen();
            if (screenName is null) return;

            try
            {
                var handle = ScreenNative.CreateDC(screenName, screenName, null, IntPtr.Zero);
                if (gammaRamp is null)
                {
                    var gammaDump = new GammaRamp();
                    if (ScreenNative.GetDeviceGammaRamp(handle, ref gammaDump))
                    {
                        gammaRamp = new DisplayGammaRamp(gammaDump);
                        //Logger.WriteLine("Gamma R: " + string.Join("-", gammaRamp.Red));
                        //Logger.WriteLine("Gamma G: " + string.Join("-", gammaRamp.Green));
                        //Logger.WriteLine("Gamma B: " + string.Join("-", gammaRamp.Blue));
                    }
                }

                if (gammaRamp is null || !gammaRamp.IsOriginal())
                {
                    Logger.WriteLine("Not default Gamma");
                    gammaRamp = new DisplayGammaRamp();
                }

                var ramp = gammaRamp.AsBrightnessRamp(bright);
                bool result = ScreenNative.SetDeviceGammaRamp(handle, ref ramp);

                Logger.WriteLine("Gamma " + bright.ToString() + ": " + result);

            }
            catch (Exception ex)
            {
                Logger.WriteLine(ex.ToString());
            }

            //ScreenBrightness.Set(60 + (int)(40 * bright));
        }

    }
}
        uses: ﻿using GHelper.Mode;

namespace GHelper.Fan
{
    public class FanSensorControl
    {
        public const int DEFAULT_FAN_MIN = 18;
        public const int DEFAULT_FAN_MAX = 58;

        public const int XGM_FAN_MAX = 72;

        public const int INADEQUATE_MAX = 104;

        const int FAN_COUNT = 3;

        Fans fansForm;
        ModeControl modeControl = Program.modeControl;

        static int[] measuredMax;
        static int sameCount = 0;

        static System.Timers.Timer timer = default!;

        static int[] _fanMax = InitFanMax();
        static bool _fanRpm = AppConfig.IsNotFalse("fan_rpm");

        public FanSensorControl(Fans fansForm)
        {
            this.fansForm = fansForm;
            timer = new System.Timers.Timer(1000);
            timer.Elapsed += Timer_Elapsed;
        }

        static int[] InitFanMax()
        {
            int[] defaultMax = GetDefaultMax();

            return new int[3] {
                AppConfig.Get("fan_max_" + (int)AsusFan.CPU, defaultMax[(int)AsusFan.CPU]),
                AppConfig.Get("fan_max_" + (int)AsusFan.GPU, defaultMax[(int)AsusFan.GPU]),
                AppConfig.Get("fan_max_" + (int)AsusFan.Mid, defaultMax[(int)AsusFan.Mid])
            };
        }


        static int[] GetDefaultMax()
        {
            if (AppConfig.ContainsModel("GA401I")) return new int[3] { 78, 76, DEFAULT_FAN_MAX };
            if (AppConfig.ContainsModel("GA401")) return new int[3] { 71, 73, DEFAULT_FAN_MAX };
            if (AppConfig.ContainsModel("GA402")) return new int[3] { 55, 56, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("G513R")) return new int[3] { 58, 60, DEFAULT_FAN_MAX };
            if (AppConfig.ContainsModel("G513Q")) return new int[3] { 69, 69, DEFAULT_FAN_MAX };
            if (AppConfig.ContainsModel("GA503")) return new int[3] { 64, 64, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("GU603")) return new int[3] { 62, 64, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("FA507R")) return new int[3] { 63, 57, DEFAULT_FAN_MAX };
            if (AppConfig.ContainsModel("FA507X")) return new int[3] { 63, 68, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("FX607J")) return new int[3] { 74, 72, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("GX650")) return new int[3] { 62, 62, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("G732")) return new int[3] { 61, 60, DEFAULT_FAN_MAX };
            if (AppConfig.ContainsModel("G713")) return new int[3] { 56, 60, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("Z301")) return new int[3] { 72, 64, DEFAULT_FAN_MAX };

            if (AppConfig.ContainsModel("GV601")) return new int[3] { 78, 59, 85 };

            if (AppConfig.ContainsModel("GA403")) return new int[3] { 68, 68, 80 };
            if (AppConfig.ContainsModel("GU605")) return new int[3] { 62, 62, 92 };

            return new int[3] { DEFAULT_FAN_MAX, DEFAULT_FAN_MAX, DEFAULT_FAN_MAX };
        }

        public static int GetFanMax(AsusFan device)
        {
            if (device == AsusFan.XGM) return XGM_FAN_MAX;

            if (_fanMax[(int)device] < 0 || _fanMax[(int)device] > INADEQUATE_MAX)
                SetFanMax(device, DEFAULT_FAN_MAX);

            return _fanMax[(int)device];
        }

        public static void SetFanMax(AsusFan device, int value)
        {
            _fanMax[(int)device] = value;
            AppConfig.Set("fan_max_" + (int)device, value);
        }

        public static bool fanRpm
        {
            get
            {
                return _fanRpm;
            }
            set
            {
                AppConfig.Set("fan_rpm", value ? 1 : 0);
                _fanRpm = value;
            }
        }

        public static string FormatFan(AsusFan device, int value)
        {
            if (value < 0) return null;

            if (value > GetFanMax(device) && value <= INADEQUATE_MAX) SetFanMax(device, value);

            if (fanRpm)
                return Properties.Strings.FanSpeed + ": " + (value * 100).ToString() + "RPM";
            else
                return Properties.Strings.FanSpeed + ": " + Math.Min(Math.Round((float)value / GetFanMax(device) * 100), 100).ToString() + "%"; // relatively to max RPM
        }

        public void StartCalibration()
        {

            measuredMax = new int[] { 0, 0, 0 };
            timer.Enabled = true;

            for (int i = 0; i < FAN_COUNT; i++)
                AppConfig.Remove("fan_max_" + i);

            Program.acpi.DeviceSet(AsusACPI.PerformanceMode, AsusACPI.PerformanceTurbo, "ModeCalibration");

            for (int i = 0; i < FAN_COUNT; i++)
                Program.acpi.SetFanCurve((AsusFan)i, new byte[] { 20, 30, 40, 50, 60, 70, 80, 90, 100, 100, 100, 100, 100, 100, 100, 100 });

        }

        private void Timer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            int fan;
            bool same = true;

            for (int i = 0; i < FAN_COUNT; i++)
            {
                fan = Program.acpi.GetFan((AsusFan)i);
                if (fan > measuredMax[i])
                {
                    measuredMax[i] = fan;
                    same = false;
                }
            }

            if (same) sameCount++;
            else sameCount = 0;

            string label = "Measuring Max Speed - CPU: " + measuredMax[(int)AsusFan.CPU] * 100 + ", GPU: " + measuredMax[(int)AsusFan.GPU] * 100;
            if (measuredMax[(int)AsusFan.Mid] > 10) label = label + ", Mid: " + measuredMax[(int)AsusFan.Mid] * 100;
            label = label + " (" + sameCount + "s)";

            fansForm.LabelFansResult(label);

            if (sameCount >= 15)
            {
                for (int i = 0; i < FAN_COUNT; i++)
                {
                    if (measuredMax[i] > 30 && measuredMax[i] < INADEQUATE_MAX) SetFanMax((AsusFan)i, measuredMax[i]);
                }

                sameCount = 0;
                FinishCalibration();
            }

        }

        private void FinishCalibration()
        {

            timer.Enabled = false;
            modeControl.SetPerformanceMode();

            string label = "Measured - CPU: " + AppConfig.Get("fan_max_" + (int)AsusFan.CPU) * 100;

            if (AppConfig.Get("fan_max_" + (int)AsusFan.GPU) > 0)
                label = label + ", GPU: " + AppConfig.Get("fan_max_" + (int)AsusFan.GPU) * 100;

            if (AppConfig.Get("fan_max_" + (int)AsusFan.Mid) > 0)
                label = label + ", Mid: " + AppConfig.Get("fan_max_" + (int)AsusFan.Mid) * 100;

            fansForm.LabelFansResult(label);
            fansForm.InitAxis();
        }
    }
}

      - name: using System.Diagnostics;
using System.Runtime.InteropServices;
using static GHelper.Gpu.AMD.Adl2.NativeMethods;

namespace GHelper.Gpu.AMD;

#region Export Struct

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
public struct ADLSGApplicationInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string strFileName;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string strFilePath;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
    public string strVersion;

    public long timeStamp;
    public uint iProfileExists;
    public uint iGPUAffinity;
    public ADLBdf GPUBdf;
}

[StructLayout(LayoutKind.Sequential)]
public struct ADLBdf
{
    public int iBus;
    public int iDevice;
    public int iFunction;
}

[StructLayout(LayoutKind.Sequential)]
public struct ADLSingleSensorData
{
    public int Supported;
    public int Value;
}

[StructLayout(LayoutKind.Sequential)]
public struct ADLPMLogDataOutput
{
    int Size;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = Adl2.ADL_PMLOG_MAX_SENSORS)]
    public ADLSingleSensorData[] Sensors;
}

[StructLayout(LayoutKind.Sequential)]
public struct ADLGcnInfo
{
    public int CuCount; //Number of compute units on the ASIC.
    public int TexCount; //Number of texture mapping units.
    public int RopCount; //Number of Render backend Units.
    public int ASICFamilyId; //Such SI, VI. See /inc/asic_reg/atiid.h for family ids
    public int ASICRevisionId; //Such as Ellesmere, Fiji.   For example - VI family revision ids are stored in /inc/asic_reg/vi_id.h
}

[Flags]
public enum ADLAsicFamilyType
{
    Undefined = 0,
    Discrete = 1 << 0,
    Integrated = 1 << 1,
    Workstation = 1 << 2,
    FireMV = 1 << 3,
    Xgp = 1 << 4,
    Fusion = 1 << 5,
    Firestream = 1 << 6,
    Embedded = 1 << 7,
}

public enum ADLSensorType
{
    SENSOR_MAXTYPES = 0,
    PMLOG_CLK_GFXCLK = 1, // Current graphic clock value in MHz
    PMLOG_CLK_MEMCLK = 2, // Current memory clock value in MHz
    PMLOG_CLK_SOCCLK = 3,
    PMLOG_CLK_UVDCLK1 = 4,
    PMLOG_CLK_UVDCLK2 = 5,
    PMLOG_CLK_VCECLK = 6,
    PMLOG_CLK_VCNCLK = 7,
    PMLOG_TEMPERATURE_EDGE = 8, // Current edge of the die temperature value in C
    PMLOG_TEMPERATURE_MEM = 9,
    PMLOG_TEMPERATURE_VRVDDC = 10,
    PMLOG_TEMPERATURE_VRMVDD = 11,
    PMLOG_TEMPERATURE_LIQUID = 12,
    PMLOG_TEMPERATURE_PLX = 13,
    PMLOG_FAN_RPM = 14, // Current fan RPM value
    PMLOG_FAN_PERCENTAGE = 15, // Current ratio of fan RPM and max RPM
    PMLOG_SOC_VOLTAGE = 16,
    PMLOG_SOC_POWER = 17,
    PMLOG_SOC_CURRENT = 18,
    PMLOG_INFO_ACTIVITY_GFX = 19, // Current graphic activity level in percentage
    PMLOG_INFO_ACTIVITY_MEM = 20, // Current memory activity level in percentage
    PMLOG_GFX_VOLTAGE = 21, // Current graphic voltage in mV
    PMLOG_MEM_VOLTAGE = 22,
    PMLOG_ASIC_POWER = 23, // Current ASIC power draw in Watt
    PMLOG_TEMPERATURE_VRSOC = 24,
    PMLOG_TEMPERATURE_VRMVDD0 = 25,
    PMLOG_TEMPERATURE_VRMVDD1 = 26,
    PMLOG_TEMPERATURE_HOTSPOT = 27, // Current center of the die temperature value in C
    PMLOG_TEMPERATURE_GFX = 28,
    PMLOG_TEMPERATURE_SOC = 29,
    PMLOG_GFX_POWER = 30,
    PMLOG_GFX_CURRENT = 31,
    PMLOG_TEMPERATURE_CPU = 32,
    PMLOG_CPU_POWER = 33,
    PMLOG_CLK_CPUCLK = 34,
    PMLOG_THROTTLER_STATUS = 35, // A bit map of GPU throttle information. If a bit is set, the bit represented type of thorttling occurred in the last metrics sampling period
    PMLOG_CLK_VCN1CLK1 = 36,
    PMLOG_CLK_VCN1CLK2 = 37,
    PMLOG_SMART_POWERSHIFT_CPU = 38,
    PMLOG_SMART_POWERSHIFT_DGPU = 39,
    PMLOG_BUS_SPEED = 40, // Current PCIE bus speed running
    PMLOG_BUS_LANES = 41, // Current PCIE bus lanes using
    PMLOG_TEMPERATURE_LIQUID0 = 42,
    PMLOG_TEMPERATURE_LIQUID1 = 43,
    PMLOG_CLK_FCLK = 44,
    PMLOG_THROTTLER_STATUS_CPU = 45,
    PMLOG_SSPAIRED_ASICPOWER = 46, // apuPower
    PMLOG_SSTOTAL_POWERLIMIT = 47, // Total Power limit    
    PMLOG_SSAPU_POWERLIMIT = 48, // APU Power limit
    PMLOG_SSDGPU_POWERLIMIT = 49, // DGPU Power limit
    PMLOG_TEMPERATURE_HOTSPOT_GCD = 50,
    PMLOG_TEMPERATURE_HOTSPOT_MCD = 51,
    PMLOG_THROTTLER_TEMP_EDGE_PERCENTAGE = 52,
    PMLOG_THROTTLER_TEMP_HOTSPOT_PERCENTAGE = 53,
    PMLOG_THROTTLER_TEMP_HOTSPOT_GCD_PERCENTAGE = 54,
    PMLOG_THROTTLER_TEMP_HOTSPOT_MCD_PERCENTAGE = 55,
    PMLOG_THROTTLER_TEMP_MEM_PERCENTAGE = 56,
    PMLOG_THROTTLER_TEMP_VR_GFX_PERCENTAGE = 57,
    PMLOG_THROTTLER_TEMP_VR_MEM0_PERCENTAGE = 58,
    PMLOG_THROTTLER_TEMP_VR_MEM1_PERCENTAGE = 59,
    PMLOG_THROTTLER_TEMP_VR_SOC_PERCENTAGE = 60,
    PMLOG_THROTTLER_TEMP_LIQUID0_PERCENTAGE = 61,
    PMLOG_THROTTLER_TEMP_LIQUID1_PERCENTAGE = 62,
    PMLOG_THROTTLER_TEMP_PLX_PERCENTAGE = 63,
    PMLOG_THROTTLER_TDC_GFX_PERCENTAGE = 64,
    PMLOG_THROTTLER_TDC_SOC_PERCENTAGE = 65,
    PMLOG_THROTTLER_TDC_USR_PERCENTAGE = 66,
    PMLOG_THROTTLER_PPT0_PERCENTAGE = 67,
    PMLOG_THROTTLER_PPT1_PERCENTAGE = 68,
    PMLOG_THROTTLER_PPT2_PERCENTAGE = 69,
    PMLOG_THROTTLER_PPT3_PERCENTAGE = 70,
    PMLOG_THROTTLER_FIT_PERCENTAGE = 71,
    PMLOG_THROTTLER_GFX_APCC_PLUS_PERCENTAGE = 72,
    PMLOG_BOARD_POWER = 73,
    PMLOG_MAX_SENSORS_REAL
};

//Throttle Status
[Flags]
public enum ADL_THROTTLE_NOTIFICATION
{
    ADL_PMLOG_THROTTLE_POWER = 1 << 0,
    ADL_PMLOG_THROTTLE_THERMAL = 1 << 1,
    ADL_PMLOG_THROTTLE_CURRENT = 1 << 2,
};

public enum ADL_PMLOG_SENSORS
{
    ADL_SENSOR_MAXTYPES = 0,
    ADL_PMLOG_CLK_GFXCLK = 1,
    ADL_PMLOG_CLK_MEMCLK = 2,
    ADL_PMLOG_CLK_SOCCLK = 3,
    ADL_PMLOG_CLK_UVDCLK1 = 4,
    ADL_PMLOG_CLK_UVDCLK2 = 5,
    ADL_PMLOG_CLK_VCECLK = 6,
    ADL_PMLOG_CLK_VCNCLK = 7,
    ADL_PMLOG_TEMPERATURE_EDGE = 8,
    ADL_PMLOG_TEMPERATURE_MEM = 9,
    ADL_PMLOG_TEMPERATURE_VRVDDC = 10,
    ADL_PMLOG_TEMPERATURE_VRMVDD = 11,
    ADL_PMLOG_TEMPERATURE_LIQUID = 12,
    ADL_PMLOG_TEMPERATURE_PLX = 13,
    ADL_PMLOG_FAN_RPM = 14,
    ADL_PMLOG_FAN_PERCENTAGE = 15,
    ADL_PMLOG_SOC_VOLTAGE = 16,
    ADL_PMLOG_SOC_POWER = 17,
    ADL_PMLOG_SOC_CURRENT = 18,
    ADL_PMLOG_INFO_ACTIVITY_GFX = 19,
    ADL_PMLOG_INFO_ACTIVITY_MEM = 20,
    ADL_PMLOG_GFX_VOLTAGE = 21,
    ADL_PMLOG_MEM_VOLTAGE = 22,
    ADL_PMLOG_ASIC_POWER = 23,
    ADL_PMLOG_TEMPERATURE_VRSOC = 24,
    ADL_PMLOG_TEMPERATURE_VRMVDD0 = 25,
    ADL_PMLOG_TEMPERATURE_VRMVDD1 = 26,
    ADL_PMLOG_TEMPERATURE_HOTSPOT = 27,
    ADL_PMLOG_TEMPERATURE_GFX = 28,
    ADL_PMLOG_TEMPERATURE_SOC = 29,
    ADL_PMLOG_GFX_POWER = 30,
    ADL_PMLOG_GFX_CURRENT = 31,
    ADL_PMLOG_TEMPERATURE_CPU = 32,
    ADL_PMLOG_CPU_POWER = 33,
    ADL_PMLOG_CLK_CPUCLK = 34,
    ADL_PMLOG_THROTTLER_STATUS = 35, // GFX
    ADL_PMLOG_CLK_VCN1CLK1 = 36,
    ADL_PMLOG_CLK_VCN1CLK2 = 37,
    ADL_PMLOG_SMART_POWERSHIFT_CPU = 38,
    ADL_PMLOG_SMART_POWERSHIFT_DGPU = 39,
    ADL_PMLOG_BUS_SPEED = 40,
    ADL_PMLOG_BUS_LANES = 41,
    ADL_PMLOG_TEMPERATURE_LIQUID0 = 42,
    ADL_PMLOG_TEMPERATURE_LIQUID1 = 43,
    ADL_PMLOG_CLK_FCLK = 44,
    ADL_PMLOG_THROTTLER_STATUS_CPU = 45,
    ADL_PMLOG_SSPAIRED_ASICPOWER = 46, // apuPower
    ADL_PMLOG_SSTOTAL_POWERLIMIT = 47, // Total Power limit
    ADL_PMLOG_SSAPU_POWERLIMIT = 48, // APU Power limit
    ADL_PMLOG_SSDGPU_POWERLIMIT = 49, // DGPU Power limit
    ADL_PMLOG_TEMPERATURE_HOTSPOT_GCD = 50,
    ADL_PMLOG_TEMPERATURE_HOTSPOT_MCD = 51,
    ADL_PMLOG_THROTTLER_TEMP_EDGE_PERCENTAGE = 52,
    ADL_PMLOG_THROTTLER_TEMP_HOTSPOT_PERCENTAGE = 53,
    ADL_PMLOG_THROTTLER_TEMP_HOTSPOT_GCD_PERCENTAGE = 54,
    ADL_PMLOG_THROTTLER_TEMP_HOTSPOT_MCD_PERCENTAGE = 55,
    ADL_PMLOG_THROTTLER_TEMP_MEM_PERCENTAGE = 56,
    ADL_PMLOG_THROTTLER_TEMP_VR_GFX_PERCENTAGE = 57,
    ADL_PMLOG_THROTTLER_TEMP_VR_MEM0_PERCENTAGE = 58,
    ADL_PMLOG_THROTTLER_TEMP_VR_MEM1_PERCENTAGE = 59,
    ADL_PMLOG_THROTTLER_TEMP_VR_SOC_PERCENTAGE = 60,
    ADL_PMLOG_THROTTLER_TEMP_LIQUID0_PERCENTAGE = 61,
    ADL_PMLOG_THROTTLER_TEMP_LIQUID1_PERCENTAGE = 62,
    ADL_PMLOG_THROTTLER_TEMP_PLX_PERCENTAGE = 63,
    ADL_PMLOG_THROTTLER_TDC_GFX_PERCENTAGE = 64,
    ADL_PMLOG_THROTTLER_TDC_SOC_PERCENTAGE = 65,
    ADL_PMLOG_THROTTLER_TDC_USR_PERCENTAGE = 66,
    ADL_PMLOG_THROTTLER_PPT0_PERCENTAGE = 67,
    ADL_PMLOG_THROTTLER_PPT1_PERCENTAGE = 68,
    ADL_PMLOG_THROTTLER_PPT2_PERCENTAGE = 69,
    ADL_PMLOG_THROTTLER_PPT3_PERCENTAGE = 70,
    ADL_PMLOG_THROTTLER_FIT_PERCENTAGE = 71,
    ADL_PMLOG_THROTTLER_GFX_APCC_PLUS_PERCENTAGE = 72,
    ADL_PMLOG_BOARD_POWER = 73,
    ADL_PMLOG_MAX_SENSORS_REAL
}

#region ADLAdapterInfo

/// <summary> ADLAdapterInfo Structure</summary>
[StructLayout(LayoutKind.Sequential)]
public struct ADLAdapterInfo
{
    /// <summary>The size of the structure</summary>
    int Size;

    /// <summary> Adapter Index</summary>
    public int AdapterIndex;

    /// <summary> Adapter UDID</summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string UDID;

    /// <summary> Adapter Bus Number</summary>
    public int BusNumber;

    /// <summary> Adapter Driver Number</summary>
    public int DriverNumber;

    /// <summary> Adapter Function Number</summary>
    public int FunctionNumber;

    /// <summary> Adapter Vendor ID</summary>
    public int VendorID;

    /// <summary> Adapter Adapter name</summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string AdapterName;

    /// <summary> Adapter Display name</summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string DisplayName;

    /// <summary> Adapter Present status</summary>
    public int Present;

    /// <summary> Adapter Exist status</summary>
    public int Exist;

    /// <summary> Adapter Driver Path</summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string DriverPath;

    /// <summary> Adapter Driver Ext Path</summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string DriverPathExt;

    /// <summary> Adapter PNP String</summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string PNPString;

    /// <summary> OS Display Index</summary>
    public int OSDisplayIndex;
}

/// <summary> ADLAdapterInfo Array</summary>
[StructLayout(LayoutKind.Sequential)]
public struct ADLAdapterInfoArray
{
    /// <summary> ADLAdapterInfo Array </summary>
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = Adl2.ADL_MAX_ADAPTERS)]
    public ADLAdapterInfo[] ADLAdapterInfo;
}

#endregion ADLAdapterInfo

#region ADLDisplayInfo

/// <summary> ADLDisplayID Structure</summary>
[StructLayout(LayoutKind.Sequential)]
public struct ADLDisplayID
{
    /// <summary> Display Logical Index </summary>
    public int DisplayLogicalIndex;

    /// <summary> Display Physical Index </summary>
    public int DisplayPhysicalIndex;

    /// <summary> Adapter Logical Index </summary>
    public int DisplayLogicalAdapterIndex;

    /// <summary> Adapter Physical Index </summary>
    public int DisplayPhysicalAdapterIndex;
}

/// <summary> ADLDisplayInfo Structure</summary>
[StructLayout(LayoutKind.Sequential)]
public struct ADLDisplayInfo
{
    /// <summary> Display Index </summary>
    public ADLDisplayID DisplayID;

    /// <summary> Display Controller Index </summary>
    public int DisplayControllerIndex;

    /// <summary> Display Name </summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string DisplayName;

    /// <summary> Display Manufacturer Name </summary>
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = Adl2.ADL_MAX_PATH)]
    public string DisplayManufacturerName;

    /// <summary> Display Type : < The Display type. CRT, TV,CV,DFP are some of display types,</summary>
    public int DisplayType;

    /// <summary> Display output type </summary>
    public int DisplayOutputType;

    /// <summary> Connector type</summary>
    public int DisplayConnector;

    ///<summary> Indicating the display info bits' mask.<summary>
    public int DisplayInfoMask;

    ///<summary> Indicating the display info value.<summary>
    public int DisplayInfoValue;
}

#endregion ADLDisplayInfo

#endregion Export Struct

public class Adl2
{
    public const string Atiadlxx_FileName = "atiadlxx.dll";

    #region Internal Constant

    /// <summary> Define the maximum path</summary>
    public const int ADL_MAX_PATH = 256;

    /// <summary> Define the maximum adapters</summary>
    public const int ADL_MAX_ADAPTERS = 40 /* 150 */;

    /// <summary> Define the maximum displays</summary>
    public const int ADL_MAX_DISPLAYS = 40 /* 150 */;

    /// <summary> Define the maximum device name length</summary>
    public const int ADL_MAX_DEVICENAME = 32;

    /// <summary> Define the successful</summary>
    public const int ADL_SUCCESS = 0;

    /// <summary> Define the failure</summary>
    public const int ADL_FAIL = -1;

    /// <summary> Define the driver ok</summary>
    public const int ADL_DRIVER_OK = 0;

    /// <summary> Maximum number of GL-Sync ports on the GL-Sync module </summary>
    public const int ADL_MAX_GLSYNC_PORTS = 8;

    /// <summary> Maximum number of GL-Sync ports on the GL-Sync module </summary>
    public const int ADL_MAX_GLSYNC_PORT_LEDS = 8;

    /// <summary> Maximum number of ADLMOdes for the adapter </summary>
    public const int ADL_MAX_NUM_DISPLAYMODES = 1024;

    /// <summary> Performance Metrics Log max sensors number </summary>
    public const int ADL_PMLOG_MAX_SENSORS = 256;

    #endregion Internal Constant

    // ///// <summary> ADL Create Function to create ADL Data</summary>
    /// <param name="enumConnectedAdapters">If it is 1, then ADL will only return the physical exist adapters </param>
    ///// <returns> retrun ADL Error Code</returns>
    public static int ADL2_Main_Control_Create(int enumConnectedAdapters, out nint adlContextHandle)
    {
        return NativeMethods.ADL2_Main_Control_Create(ADL_Main_Memory_Alloc_Impl_Reference, enumConnectedAdapters, out adlContextHandle);
    }

    public static void FreeMemory(nint buffer)
    {
        Memory_Free_Impl(buffer);
    }

    private static bool? isDllLoaded;

    public static bool Load()
    {
        if (isDllLoaded != null)
            return isDllLoaded.Value;

        try
        {
            Marshal.PrelinkAll(typeof(Adl2));
            isDllLoaded = true;
        }
        catch (Exception e) when (e is DllNotFoundException or EntryPointNotFoundException)
        {
            Debug.WriteLine(e);
            isDllLoaded = false;
        }

        return isDllLoaded.Value;
    }

    private static ADL_Main_Memory_Alloc ADL_Main_Memory_Alloc_Impl_Reference = Memory_Alloc_Impl;

    /// <summary> Build in memory allocation function</summary>
    /// <param name="size">input size</param>
    /// <returns>return the memory buffer</returns>
    private static nint Memory_Alloc_Impl(int size)
    {
        return Marshal.AllocCoTaskMem(size);
    }

    /// <summary> Build in memory free function</summary>
    /// <param name="buffer">input buffer</param>
    private static void Memory_Free_Impl(nint buffer)
    {
        if (nint.Zero != buffer)
        {
            Marshal.FreeCoTaskMem(buffer);
        }
    }

    public static class NativeMethods
    {
        /// <summary> ADL Memory allocation function allows ADL to callback for memory allocation</summary>
        /// <param name="size">input size</param>
        /// <returns> retrun ADL Error Code</returns>
        public delegate nint ADL_Main_Memory_Alloc(int size);

        // ///// <summary> ADL Create Function to create ADL Data</summary>
        /// <param name="callback">Call back functin pointer which is ised to allocate memeory </param>
        /// <param name="enumConnectedAdapters">If it is 1, then ADL will only retuen the physical exist adapters </param>
        ///// <returns> retrun ADL Error Code</returns>
        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Main_Control_Create(ADL_Main_Memory_Alloc callback, int enumConnectedAdapters, out nint adlContextHandle);

        /// <summary> ADL Destroy Function to free up ADL Data</summary>
        /// <returns> retrun ADL Error Code</returns>
        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Main_Control_Destroy(nint adlContextHandle);

        /// <summary> ADL Function to get the number of adapters</summary>
        /// <param name="numAdapters">return number of adapters</param>
        /// <returns> retrun ADL Error Code</returns>
        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_NumberOfAdapters_Get(nint adlContextHandle, out int numAdapters);

        /// <summary> ADL Function to get the GPU adapter information</summary>
        /// <param name="info">return GPU adapter information</param>
        /// <param name="inputSize">the size of the GPU adapter struct</param>
        /// <returns> retrun ADL Error Code</returns>
        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_AdapterInfo_Get(nint adlContextHandle, nint info, int inputSize);

        /// <summary> Function to determine if the adapter is active or not.</summary>
        /// <remarks>The function is used to check if the adapter associated with iAdapterIndex is active</remarks>  
        /// <param name="adapterIndex"> Adapter Index.</param>
        /// <param name="status"> Status of the adapter. True: Active; False: Dsiabled</param>
        /// <returns>Non zero is successfull</returns> 
        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_Active_Get(nint adlContextHandle, int adapterIndex, out int status);

        /// <summary>Get display information based on adapter index</summary>
        /// <param name="adapterIndex">Adapter Index</param>
        /// <param name="numDisplays">return the total number of supported displays</param>
        /// <param name="displayInfoArray">return ADLDisplayInfo Array for supported displays' information</param>
        /// <param name="forceDetect">force detect or not</param>
        /// <returns>return ADL Error Code</returns>
        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Display_DisplayInfo_Get(
            nint adlContextHandle,
            int adapterIndex,
            out int numDisplays,
            out nint displayInfoArray,
            int forceDetect
        );

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Overdrive_Caps(
            nint adlContextHandle,
            int adapterIndex,
            out int supported,
            out int enabled,
            out int version
        );

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_New_QueryPMLogData_Get(nint adlContextHandle, int adapterIndex, out ADLPMLogDataOutput adlpmLogDataOutput);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_ASICFamilyType_Get(nint adlContextHandle, int adapterIndex, out ADLAsicFamilyType asicFamilyType, out int asicFamilyTypeValids);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_SwitchableGraphics_Applications_Get(
            nint context,
            int iListType,
            out int lpNumApps,
            out nint lppAppList);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_VariBright_Caps(
            nint context,
            int iAdapterIndex,
            out int iSupported,
            out int iEnabled,
            out int iVersion);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_VariBrightEnable_Set(
            nint context,
            int iAdapterIndex,
            int iEnabled);

        // FPS

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_FrameMetrics_Start(
            IntPtr context,
            int iAdapterIndex,
            int VidPnSourceId
        );

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_FrameMetrics_Stop(
            IntPtr context,
            int iAdapterIndex,
            int VidPnSourceId
        );

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_Adapter_FrameMetrics_Get(
              IntPtr context,
              int iAdapterIndex,
              int VidPnSourceId,
              out float iFramesPerSecond
          );

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_FPS_Settings_Get(IntPtr context, int iAdapterIndex, out ADLFPSSettingsOutput lpFPSSettings);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_FPS_Settings_Set(IntPtr context, int iAdapterIndex, ADLFPSSettingsInput lpFPSSettings);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_FPS_Settings_Reset(IntPtr context, int iAdapterIndex);

        [StructLayout(LayoutKind.Sequential)]
        public struct ADLFPSSettingsOutput
        {
            public int ulSize;
            public int bACFPSEnabled;
            public int bDCFPSEnabled;
            public int ulACFPSCurrent;
            public int ulDCFPSCurrent;
            public int ulACFPSMaximum;
            public int ulACFPSMinimum;
            public int ulDCFPSMaximum;
            public int ulDCFPSMinimum;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ADLFPSSettingsInput
        {
            public int ulSize;
            public int bGlobalSettings;
            public int ulACFPSCurrent;
            public int ulDCFPSCurrent;

            // Assuming ulReserved is an array of 6 integers
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
            public int[] ulReserved;
        }
        // Clocks

        [StructLayout(LayoutKind.Sequential)]
        public struct ADLODNPerformanceLevel
        {
            public int iClock;
            public int iVddc;
            public int iEnabled;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ADLODNPerformanceLevels
        {
            public int iSize;
            public int iMode;
            public int iNumberOfPerformanceLevels;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 1)]
            public ADLODNPerformanceLevel[] aLevels;
        }


        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_OverdriveN_SystemClocks_Get(
            nint context,
            int adapterIndex,
            ref ADLODNPerformanceLevels performanceLevels);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_OverdriveN_SystemClocks_Set(
            nint context,
            int adapterIndex,
            ref ADLODNPerformanceLevels performanceLevels);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_OverdriveN_MemoryClocks_Get(
            nint context,
            int adapterIndex,
            ref ADLODNPerformanceLevels performanceLevels);

        [DllImport(Atiadlxx_FileName)]
        public static extern int ADL2_OverdriveN_MemoryClocks_Set(
            nint context,
            int adapterIndex,
            ref ADLODNPerformanceLevels performanceLevels);
    }
}
        run: cmake -B ${{ using GHelper.Helpers;
using System.Runtime.InteropServices;
using static GHelper.Gpu.AMD.Adl2.NativeMethods;

namespace GHelper.Gpu.AMD;

// Reference: https://github.com/GPUOpen-LibrariesAndSDKs/display-library/blob/master/Sample-Managed/Program.cs
public class AmdGpuControl : IGpuControl
{
    private bool _isReady;
    private nint _adlContextHandle;

    private readonly ADLAdapterInfo _internalDiscreteAdapter;
    private readonly ADLAdapterInfo? _iGPU;

    public bool IsNvidia => false;

    public string FullName => _internalDiscreteAdapter!.AdapterName;

    private ADLAdapterInfo? FindByType(ADLAsicFamilyType type = ADLAsicFamilyType.Discrete)
    {
        ADL2_Adapter_NumberOfAdapters_Get(_adlContextHandle, out int numberOfAdapters);
        if (numberOfAdapters <= 0)
            return null;

        ADLAdapterInfoArray osAdapterInfoData = new();
        int osAdapterInfoDataSize = Marshal.SizeOf(osAdapterInfoData);
        nint AdapterBuffer = Marshal.AllocCoTaskMem(osAdapterInfoDataSize);
        Marshal.StructureToPtr(osAdapterInfoData, AdapterBuffer, false);
        if (ADL2_Adapter_AdapterInfo_Get(_adlContextHandle, AdapterBuffer, osAdapterInfoDataSize) != Adl2.ADL_SUCCESS)
            return null;

        osAdapterInfoData = (ADLAdapterInfoArray)Marshal.PtrToStructure(AdapterBuffer, osAdapterInfoData.GetType())!;

        const int amdVendorId = 1002;

        // Determine which GPU is internal discrete AMD GPU
        ADLAdapterInfo internalDiscreteAdapter =
            osAdapterInfoData.ADLAdapterInfo
                .FirstOrDefault(adapter =>
                {
                    if (adapter.Exist == 0 || adapter.Present == 0)
                        return false;

                    if (adapter.VendorID != amdVendorId)
                        return false;

                    if (ADL2_Adapter_ASICFamilyType_Get(_adlContextHandle, adapter.AdapterIndex, out ADLAsicFamilyType asicFamilyType, out int asicFamilyTypeValids) != Adl2.ADL_SUCCESS)
                        return false;

                    asicFamilyType = (ADLAsicFamilyType)((int)asicFamilyType & asicFamilyTypeValids);

                    return (asicFamilyType & type) != 0;
                });

        if (internalDiscreteAdapter.Exist == 0)
            return null;

        return internalDiscreteAdapter;

    }

    public AmdGpuControl()
    {
        if (!Adl2.Load())
            return;

        try
        {
            if (Adl2.ADL2_Main_Control_Create(1, out _adlContextHandle) != Adl2.ADL_SUCCESS) return;
        } catch (Exception ex)
        {
            Logger.WriteLine(ex.Message);
            return;
        }

        ADLAdapterInfo? internalDiscreteAdapter = FindByType(ADLAsicFamilyType.Discrete);

        if (internalDiscreteAdapter is not null)
        {
            _internalDiscreteAdapter = (ADLAdapterInfo)internalDiscreteAdapter;
            _isReady = true;
        }

        _iGPU = FindByType(ADLAsicFamilyType.Integrated);

    }

    public bool IsValid => _isReady && _adlContextHandle != nint.Zero;

    public int? GetCurrentTemperature()
    {
        if (!IsValid)
            return null;

        if (ADL2_New_QueryPMLogData_Get(_adlContextHandle, _internalDiscreteAdapter.AdapterIndex, out ADLPMLogDataOutput adlpmLogDataOutput) != Adl2.ADL_SUCCESS)
            return null;

        ADLSingleSensorData temperatureSensor = adlpmLogDataOutput.Sensors[(int)ADLSensorType.PMLOG_TEMPERATURE_EDGE];
        if (temperatureSensor.Supported == 0)
            return null;

        return temperatureSensor.Value;
    }


    public int? GetGpuUse()
    {
        if (!IsValid) return null;

        if (ADL2_New_QueryPMLogData_Get(_adlContextHandle, _internalDiscreteAdapter.AdapterIndex, out ADLPMLogDataOutput adlpmLogDataOutput) != Adl2.ADL_SUCCESS)
            return null;

        ADLSingleSensorData gpuUsage = adlpmLogDataOutput.Sensors[(int)ADLSensorType.PMLOG_INFO_ACTIVITY_GFX];
        if (gpuUsage.Supported == 0)
            return null;

        return gpuUsage.Value;

    }

    public int? GetGpuPower()
    {
        if (_adlContextHandle == nint.Zero || _iGPU == null) return null;
        if (ADL2_New_QueryPMLogData_Get(_adlContextHandle, ((ADLAdapterInfo)_iGPU).AdapterIndex, out ADLPMLogDataOutput adlpmLogDataOutput) != Adl2.ADL_SUCCESS) return null;

        ADLSingleSensorData gpuUsage = adlpmLogDataOutput.Sensors[(int)ADLSensorType.PMLOG_ASIC_POWER];
        if (gpuUsage.Supported == 0) return null;

        return gpuUsage.Value;

    }


    public bool SetVariBright(int enabled)
    {
        if (_adlContextHandle == nint.Zero) return false;

        ADLAdapterInfo? iGPU = FindByType(ADLAsicFamilyType.Integrated);
        if (iGPU is null) return false;

        return ADL2_Adapter_VariBrightEnable_Set(_adlContextHandle, ((ADLAdapterInfo)iGPU).AdapterIndex, enabled) == Adl2.ADL_SUCCESS;

    }

    public bool GetVariBright(out int supported, out int enabled)
    {
        supported = enabled = -1;

        if (_adlContextHandle == nint.Zero) return false;

        ADLAdapterInfo? iGPU = FindByType(ADLAsicFamilyType.Integrated);
        if (iGPU is null) return false;

        if (ADL2_Adapter_VariBright_Caps(_adlContextHandle, ((ADLAdapterInfo)iGPU).AdapterIndex, out int supportedOut, out int enabledOut, out int version) != Adl2.ADL_SUCCESS)
            return false;

        supported = supportedOut;
        enabled = enabledOut;

        return true;
    }

    public void StartFPS()
    {
        if (_adlContextHandle == nint.Zero || _iGPU == null) return;
        ADL2_Adapter_FrameMetrics_Start(_adlContextHandle, ((ADLAdapterInfo)_iGPU).AdapterIndex, 0);
    }

    public void StopFPS()
    {
        if (_adlContextHandle == nint.Zero || _iGPU == null) return;
        ADL2_Adapter_FrameMetrics_Stop(_adlContextHandle, ((ADLAdapterInfo)_iGPU).AdapterIndex, 0);
    }

    public float GetFPS()
    {
        if (_adlContextHandle == nint.Zero || _iGPU == null) return 0;
        float fps;
        if (ADL2_Adapter_FrameMetrics_Get(_adlContextHandle, ((ADLAdapterInfo)_iGPU).AdapterIndex, 0, out fps) != Adl2.ADL_SUCCESS) return 0;
        return fps;
    }

    public int GetFPSLimit()
    {
        if (_adlContextHandle == nint.Zero || _iGPU == null) return -1;
        ADLFPSSettingsOutput settings;
        if (ADL2_FPS_Settings_Get(_adlContextHandle, ((ADLAdapterInfo)_iGPU).AdapterIndex, out settings) != Adl2.ADL_SUCCESS) return -1;

        Logger.WriteLine($"FPS Limit: {settings.ulACFPSCurrent}");

        return settings.ulACFPSCurrent;
    }

    public int SetFPSLimit(int limit)
    {
        if (_adlContextHandle == nint.Zero || _iGPU == null) return -1;

        ADLFPSSettingsInput settings = new ADLFPSSettingsInput();

        settings.ulACFPSCurrent = limit;
        settings.ulDCFPSCurrent = limit;
        settings.bGlobalSettings = 1;

        if (ADL2_FPS_Settings_Set(_adlContextHandle, ((ADLAdapterInfo)_iGPU).AdapterIndex, settings) != Adl2.ADL_SUCCESS) return 0;

        return 1;
    }

    public ADLODNPerformanceLevels? GetGPUClocks()
    {
        if (!IsValid) return null;

        ADLODNPerformanceLevels performanceLevels = new();
        ADL2_OverdriveN_SystemClocks_Get(_adlContextHandle, _internalDiscreteAdapter.AdapterIndex, ref performanceLevels);

        return performanceLevels;
    }

    public void KillGPUApps()
    {

        if (!IsValid) return;

        nint appInfoPtr = nint.Zero;
        int appCount = 0;

        try
        {
            // Get switchable graphics applications information
            var result = ADL2_SwitchableGraphics_Applications_Get(_adlContextHandle, 2, out appCount, out appInfoPtr);
            if (result != 0)
            {
                throw new Exception("Failed to get switchable graphics applications. Error code: " + result);
            }

            // Convert the application data pointers to an array of structs
            var appInfoArray = new ADLSGApplicationInfo[appCount];
            nint currentPtr = appInfoPtr;

            for (int i = 0; i < appCount; i++)
            {
                appInfoArray[i] = Marshal.PtrToStructure<ADLSGApplicationInfo>(currentPtr);
                currentPtr = nint.Add(currentPtr, Marshal.SizeOf<ADLSGApplicationInfo>());
            }

            var appNames = new List<string>();

            for (int i = 0; i < appCount; i++)
            {
                if (appInfoArray[i].iGPUAffinity == 1)
                {
                    Logger.WriteLine(appInfoArray[i].strFileName + ":" + appInfoArray[i].iGPUAffinity + "(" + appInfoArray[i].timeStamp + ")");
                    appNames.Add(Path.GetFileNameWithoutExtension(appInfoArray[i].strFileName));
                }
            }

            List<string> immune = new() { "svchost", "system", "ntoskrnl", "csrss", "winlogon", "wininit", "smss" };

            foreach (string kill in appNames)
                if (!immune.Contains(kill.ToLower()))
                    ProcessHelper.KillByName(kill);


        }
        catch (Exception ex)
        {
            Logger.WriteLine(ex.Message);
        }
        finally
        {
            // Clean up resources
            if (appInfoPtr != nint.Zero)
            {
                Marshal.FreeCoTaskMem(appInfoPtr);
            }

        }
    }


    private void ReleaseUnmanagedResources()
    {
        if (_adlContextHandle != nint.Zero)
        {
            ADL2_Main_Control_Destroy(_adlContextHandle);
            _adlContextHandle = nint.Zero;
            _isReady = false;
        }
    }

    public void Dispose()
    {
        ReleaseUnmanagedResources();
        GC.SuppressFinalize(this);
    }

    ~AmdGpuControl()
    {
        ReleaseUnmanagedResources();
    }
} }}

      # Build is not required unless generated source files are used
      # - name: ﻿using GHelper.Helpers;
using NvAPIWrapper.GPU;
using NvAPIWrapper.Native;
using NvAPIWrapper.Native.GPU;
using NvAPIWrapper.Native.GPU.Structures;
using NvAPIWrapper.Native.Interfaces.GPU;
using System.Diagnostics;
using static NvAPIWrapper.Native.GPU.Structures.PerformanceStates20InfoV1;

namespace GHelper.Gpu.NVidia;

public class NvidiaGpuControl : IGpuControl
{

    public static int MaxCoreOffset => AppConfig.Get("max_gpu_core", 250);
    public static int MaxMemoryOffset => AppConfig.Get("max_gpu_memory", 500);

    public static int MinCoreOffset = AppConfig.Get("min_gpu_core", -250);
    public static int MinMemoryOffset = AppConfig.Get("min_gpu_memory", -500);

    public static int MinClockLimit = AppConfig.Get("min_gpu_clock", 400);
    public const int MaxClockLimit = 3000;

    private static PhysicalGPU? _internalGpu;

    public NvidiaGpuControl()
    {
        _internalGpu = GetInternalDiscreteGpu();
    }

    public bool IsValid => _internalGpu != null;

    public bool IsNvidia => IsValid;

    public string FullName => _internalGpu!.FullName;

    public int? GetCurrentTemperature()
    {
        if (!IsValid) return null;

        PhysicalGPU internalGpu = _internalGpu!;
        IThermalSensor? gpuSensor =
            GPUApi.GetThermalSettings(internalGpu.Handle).Sensors
            .FirstOrDefault(s => s.Target == ThermalSettingsTarget.GPU);

        return gpuSensor?.CurrentTemperature;
    }

    public void Dispose()
    {
    }

    public void KillGPUApps()
    {

        if (!IsValid) return;
        PhysicalGPU internalGpu = _internalGpu!;

        try
        {
            Process[] processes = internalGpu.GetActiveApplications();
            foreach (Process process in processes)
                try
                {
                    Logger.WriteLine("Kill:" + process.ProcessName);
                    ProcessHelper.KillByProcess(process);
                }
                catch (Exception ex)
                {
                    Logger.WriteLine(ex.Message);
                }
        }
        catch (Exception ex)
        {
            Logger.WriteLine(ex.Message);
        }

        //GeneralApi.RestartDisplayDriver();
    }


    public bool GetClocks(out int core, out int memory)
    {
        PhysicalGPU internalGpu = _internalGpu!;

        //Logger.WriteLine(internalGpu.FullName);
        //Logger.WriteLine(internalGpu.ArchitectInformation.ToString());

        try
        {
            IPerformanceStates20Info states = GPUApi.GetPerformanceStates20(internalGpu.Handle);
            core = states.Clocks[PerformanceStateId.P0_3DPerformance][0].FrequencyDeltaInkHz.DeltaValue / 1000;
            memory = states.Clocks[PerformanceStateId.P0_3DPerformance][1].FrequencyDeltaInkHz.DeltaValue / 1000;
            Logger.WriteLine($"GET GPU CLOCKS: {core}, {memory}");

            foreach (var delta in states.Voltages[PerformanceStateId.P0_3DPerformance])
            {
                Logger.WriteLine("GPU VOLT:" + delta.IsEditable + " - " + delta.ValueDeltaInMicroVolt.DeltaValue);
            }

            return true;

        }
        catch (Exception ex)
        {
            Logger.WriteLine("GET GPU CLOCKS:" + ex.Message);
            core = memory = 0;
            return false;
        }

    }


    private bool RunPowershellCommand(string script)
    {
        try
        {
            ProcessHelper.RunCMD("powershell", script);
            return true;
        }
        catch (Exception ex)
        {
            Logger.WriteLine(ex.ToString());
            return false;
        }

    }

    public int GetMaxGPUCLock()
    {
        PhysicalGPU internalGpu = _internalGpu!;
        try
        {
            PrivateClockBoostLockV2 data = GPUApi.GetClockBoostLock(internalGpu.Handle);
            int limit = (int)data.ClockBoostLocks[0].VoltageInMicroV / 1000;
            Logger.WriteLine("GET CLOCK LIMIT: " + limit);
            return limit;
        }
        catch (Exception ex)
        {
            Logger.WriteLine("GET CLOCK LIMIT: " + ex.Message);
            return -1;

        }
    }


    public int SetMaxGPUClock(int clock)
    {

        if (clock < MinClockLimit || clock >= MaxClockLimit) clock = 0;

        int _clockLimit = GetMaxGPUCLock();

        if (_clockLimit < 0 && clock == 0) return 0;

        if (_clockLimit != clock)
        {
            if (clock > 0) RunPowershellCommand($"nvidia-smi -lgc 0,{clock}");
            else RunPowershellCommand($"nvidia-smi -rgc");
            return 1;
        }
        else
        {
            return 0;
        }


    }

    public bool RestartGPU()
    {
        return RunPowershellCommand(@"$device = Get-PnpDevice | Where-Object { $_.FriendlyName -imatch 'NVIDIA' -and $_.Class -eq 'Display' }; Disable-PnpDevice $device.InstanceId -Confirm:$false; Start-Sleep -Seconds 5; Enable-PnpDevice $device.InstanceId -Confirm:$false");
    }


    public int SetClocks(int core, int memory)
    {

        if (core < MinCoreOffset || core > MaxCoreOffset) return 0;
        if (memory < MinMemoryOffset || memory > MaxMemoryOffset) return 0;

        GetClocks(out int currentCore, out int currentMemory);

        // Nothing to set
        if (Math.Abs(core - currentCore) < 5 && Math.Abs(memory - currentMemory) < 5) return 0;

        PhysicalGPU internalGpu = _internalGpu!;

        var coreClock = new PerformanceStates20ClockEntryV1(PublicClockDomain.Graphics, new PerformanceStates20ParameterDelta(core * 1000));
        var memoryClock = new PerformanceStates20ClockEntryV1(PublicClockDomain.Memory, new PerformanceStates20ParameterDelta(memory * 1000));
        //var voltageEntry = new PerformanceStates20BaseVoltageEntryV1(PerformanceVoltageDomain.Core, new PerformanceStates20ParameterDelta(voltage));

        PerformanceStates20ClockEntryV1[] clocks = { coreClock, memoryClock };
        PerformanceStates20BaseVoltageEntryV1[] voltages = { };

        PerformanceState20[] performanceStates = { new PerformanceState20(PerformanceStateId.P0_3DPerformance, clocks, voltages) };

        var overclock = new PerformanceStates20InfoV1(performanceStates, 2, 0);

        try
        {
            Logger.WriteLine($"SET GPU CLOCKS: {core}, {memory}");
            GPUApi.SetPerformanceStates20(internalGpu.Handle, overclock);
        }
        catch (Exception ex)
        {
            Logger.WriteLine("SET GPU CLOCKS: " + ex.Message);
            return -1;
        }

        return 1;
    }

    private static PhysicalGPU? GetInternalDiscreteGpu()
    {
        try
        {
            return PhysicalGPU
                .GetPhysicalGPUs()
                .FirstOrDefault(gpu => gpu.SystemType == SystemType.Laptop);
        }
        catch (Exception ex)
        {
            Debug.WriteLine(ex.Message);
            return null;
        }
    }


    public int? GetGpuUse()
    {
        if (!IsValid)
            return null;

        PhysicalGPU internalGpu = _internalGpu!;
        IUtilizationDomainInfo? gpuUsage = GPUApi.GetUsages(internalGpu.Handle).GPU;

        return (int?)gpuUsage?.Percentage;

    }

}
      #   run: cmake --build ${{ ﻿using Ryzen;
using System.Diagnostics;
using System.Text.RegularExpressions;

public static class NvidiaSmi
{
    public static bool GetDisplayActiveStatus()
    {
        // Non AMD devices doesn't seem to be affected
        if (!RyzenControl.IsAMD()) return false;

        string commandOutput = RunNvidiaSmiCommand();

        Logger.WriteLine(commandOutput);

        if (commandOutput.Length == 0) return false;
        if (!commandOutput.Contains("RTX 40")) return false;

        // Extract the "Display Active" status using regular expressions
        string displayActivePattern = @"Display Active\s+:\s+(\w+)";

        Match match = Regex.Match(commandOutput, displayActivePattern, RegexOptions.IgnoreCase);

        if (match.Success)
        {
            string status = match.Groups[1].Value.ToLower().Trim(' ');
            return status == "enabled";
        }

        return false; // Return false if the "Display Active" status is not found
    }

    public static int GetDefaultMaxGPUPower()
    {
        if (AppConfig.ContainsModel("GU605")) return 125;
        if (AppConfig.ContainsModel("GA403")) return 90;
        if (AppConfig.ContainsModel("FA607")) return 140;
        else return 175;
    }

    public static int GetMaxGPUPower()
    {
        string output = RunNvidiaSmiCommand("--query-gpu=power.max_limit --format csv,noheader,nounits");
        output = output.Trim().Trim('\n', '\r');

        if (float.TryParse(output, out float floatValue))
        {
            int intValue = (int)floatValue;
            if (intValue >= 50 && intValue <= 175) return intValue;
        }

        return GetDefaultMaxGPUPower();
    }

    private static string RunNvidiaSmiCommand(string arguments = "-i 0 -q")
    {
        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            FileName = "nvidia-smi",
            Arguments = arguments,
            RedirectStandardOutput = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        Process process = new Process
        {
            StartInfo = startInfo
        };

        try
        {
            process.Start();
            string output = process.StandardOutput.ReadToEnd();
            process.WaitForExit();
            return output;
        }
        catch (Exception ex)
        {
            //return File.ReadAllText(@"smi.txt");
            Debug.WriteLine(ex.Message);
        }

        return "";

    }
} }}

      - name: ﻿using GHelper.Display;
using GHelper.Gpu.NVidia;
using GHelper.Helpers;
using GHelper.USB;
using System.Diagnostics;

namespace GHelper.Gpu
{
    public class GPUModeControl
    {
        SettingsForm settings;
        ScreenControl screenControl = new ScreenControl();

        public static int gpuMode;
        public static bool? gpuExists = null;


        public GPUModeControl(SettingsForm settingsForm)
        {
            settings = settingsForm;
        }

        public void InitGPUMode()
        {
            int eco = Program.acpi.DeviceGet(AsusACPI.GPUEco);
            int mux = Program.acpi.DeviceGet(AsusACPI.GPUMux);

            if (mux < 0) mux = Program.acpi.DeviceGet(AsusACPI.GPUMuxVivo);

            Logger.WriteLine("Eco flag : " + eco);
            Logger.WriteLine("Mux flag : " + mux);

            settings.VisualiseGPUButtons(eco >= 0, mux >= 0);

            if (mux == 0)
            {
                gpuMode = AsusACPI.GPUModeUltimate;
            }
            else
            {
                if (eco == 1)
                    gpuMode = AsusACPI.GPUModeEco;
                else
                    gpuMode = AsusACPI.GPUModeStandard;

                // GPU mode not supported
                if (eco < 0 && mux < 0)
                {
                    if (gpuExists is null) gpuExists = Program.acpi.GetFan(AsusFan.GPU) >= 0;
                    settings.HideGPUModes((bool)gpuExists);
                }
            }

            AppConfig.Set("gpu_mode", gpuMode);
            settings.VisualiseGPUMode(gpuMode);

            Aura.CustomRGB.ApplyGPUColor();

        }



        public void SetGPUMode(int GPUMode, int auto = 0)
        {

            int CurrentGPU = AppConfig.Get("gpu_mode");
            AppConfig.Set("gpu_auto", auto);

            if (CurrentGPU == GPUMode)
            {
                settings.VisualiseGPUMode();
                return;
            }

            var restart = false;
            var changed = false;

            int status;

            if (CurrentGPU == AsusACPI.GPUModeUltimate)
            {
                DialogResult dialogResult = MessageBox.Show(Properties.Strings.AlertUltimateOff, Properties.Strings.AlertUltimateTitle, MessageBoxButtons.YesNo);
                if (dialogResult == DialogResult.Yes)
                {
                    status = Program.acpi.DeviceSet(AsusACPI.GPUMux, 1, "GPUMux");
                    if (status != 1) Program.acpi.DeviceSet(AsusACPI.GPUMuxVivo, 1, "GPUMuxVivo");
                    restart = true;
                    changed = true;
                }
            }
            else if (GPUMode == AsusACPI.GPUModeUltimate)
            {
                DialogResult dialogResult = MessageBox.Show(Properties.Strings.AlertUltimateOn, Properties.Strings.AlertUltimateTitle, MessageBoxButtons.YesNo);
                if (dialogResult == DialogResult.Yes)
                {
                    if (AppConfig.NoAutoUltimate())
                    {
                        Program.acpi.SetGPUEco(0);
                        Thread.Sleep(100);
                    }
                    status = Program.acpi.DeviceSet(AsusACPI.GPUMux, 0, "GPUMux");
                    if (status != 1) Program.acpi.DeviceSet(AsusACPI.GPUMuxVivo, 0, "GPUMuxVivo");
                    restart = true;
                    changed = true;
                }

            }
            else if (GPUMode == AsusACPI.GPUModeEco)
            {
                settings.VisualiseGPUMode(GPUMode);
                SetGPUEco(1, true);
                changed = true;
            }
            else if (GPUMode == AsusACPI.GPUModeStandard)
            {
                settings.VisualiseGPUMode(GPUMode);
                SetGPUEco(0);
                changed = true;
            }

            if (changed)
            {
                AppConfig.Set("gpu_mode", GPUMode);
            }

            if (restart)
            {
                settings.VisualiseGPUMode();
                Process.Start("shutdown", "/r /t 1");
            }

        }



        public void SetGPUEco(int eco, bool hardWay = false)
        {

            settings.LockGPUModes();

            Task.Run(async () =>
            {

                int status = 1;

                if (eco == 1)
                {
                    HardwareControl.KillGPUApps();
                }

                Logger.WriteLine($"Running eco command {eco}");

                status = Program.acpi.SetGPUEco(eco);

                if (status == 0 && eco == 1 && hardWay) RestartGPU();

                await Task.Delay(TimeSpan.FromMilliseconds(AppConfig.Get("refresh_delay", 500)));

                settings.Invoke(delegate
                {
                    InitGPUMode();
                    screenControl.AutoScreen();
                });

                if (eco == 0)
                {
                    await Task.Delay(TimeSpan.FromMilliseconds(3000));
                    HardwareControl.RecreateGpuControl();
                    Program.modeControl.SetGPUClocks(false);
                }

                if (AppConfig.Is("mode_reapply"))
                {
                    await Task.Delay(TimeSpan.FromMilliseconds(3000));
                    Program.modeControl.AutoPerformance();
                }

            });


        }

        public static bool IsPlugged()
        {
            if (SystemInformation.PowerStatus.PowerLineStatus != PowerLineStatus.Online) return false;
            if (!AppConfig.Is("optimized_usbc")) return true;

            if (AppConfig.ContainsModel("FA507")) Thread.Sleep(1000);

            int chargerMode = Program.acpi.DeviceGet(AsusACPI.ChargerMode);
            Logger.WriteLine("ChargerStatus: " + chargerMode);

            if (chargerMode <= 0) return true;
            return (chargerMode & AsusACPI.ChargerBarrel) > 0;

        }

        public bool AutoGPUMode(bool optimized = false)
        {

            bool GpuAuto = AppConfig.Is("gpu_auto");
            bool ForceGPU = AppConfig.IsForceSetGPUMode();

            int GpuMode = AppConfig.Get("gpu_mode");

            if (!GpuAuto && !ForceGPU) return false;

            int eco = Program.acpi.DeviceGet(AsusACPI.GPUEco);
            int mux = Program.acpi.DeviceGet(AsusACPI.GPUMux);

            if (mux == 0)
            {
                if (optimized) SetGPUMode(AsusACPI.GPUModeStandard, 1);
                return false;
            }
            else
            {

                if (eco == 1)
                    if ((GpuAuto && IsPlugged()) || (ForceGPU && GpuMode == AsusACPI.GPUModeStandard))
                    {
                        SetGPUEco(0);
                        return true;
                    }
                if (eco == 0)
                    if ((GpuAuto && !IsPlugged()) || (ForceGPU && GpuMode == AsusACPI.GPUModeEco))
                    {

                        if (HardwareControl.IsUsedGPU())
                        {
                            DialogResult dialogResult = MessageBox.Show(Properties.Strings.AlertDGPU, Properties.Strings.AlertDGPUTitle, MessageBoxButtons.YesNo);
                            if (dialogResult == DialogResult.No) return false;
                        }

                        SetGPUEco(1);
                        return true;
                    }
            }

            return false;

        }


        public void RestartGPU(bool confirm = true)
        {
            if (HardwareControl.GpuControl is null) return;
            if (!HardwareControl.GpuControl!.IsNvidia) return;

            if (confirm)
            {
                DialogResult dialogResult = MessageBox.Show(Properties.Strings.RestartGPU, Properties.Strings.EcoMode, MessageBoxButtons.YesNo);
                if (dialogResult == DialogResult.No) return;
            }

            ProcessHelper.RunAsAdmin("gpurestart");

            if (!ProcessHelper.IsUserAdministrator()) return;

            Logger.WriteLine("Trying to restart dGPU");

            Task.Run(async () =>
            {
                settings.LockGPUModes("Restarting GPU ...");

                var nvControl = (NvidiaGpuControl)HardwareControl.GpuControl;
                bool status = nvControl.RestartGPU();

                settings.Invoke(delegate
                {
                    //labelTipGPU.Text = status ? "GPU Restarted, you can try Eco mode again" : "Failed to restart GPU"; TODO
                    InitGPUMode();
                });
            });

        }


        public void InitXGM()
        {
            if (Program.acpi.IsXGConnected())
            {
                //Program.acpi.DeviceSet(AsusACPI.GPUXGInit, 1, "XG Init");
                XGM.Init();
            }

        }

        public void ToggleXGM(bool silent = false)
        {

            Task.Run(async () =>
            {
                settings.LockGPUModes();

                if (Program.acpi.DeviceGet(AsusACPI.GPUXG) == 1)
                {
                    XGM.Reset();
                    HardwareControl.KillGPUApps();

                    if (silent)
                    {
                        Program.acpi.DeviceSet(AsusACPI.GPUXG, 0, "GPU XGM");
                        await Task.Delay(TimeSpan.FromSeconds(15));
                    }
                    else
                    {
                        DialogResult dialogResult = MessageBox.Show("Did you close all applications running on XG Mobile?", "Disabling XG Mobile", MessageBoxButtons.YesNo);
                        if (dialogResult == DialogResult.Yes)
                        {
                            Program.acpi.DeviceSet(AsusACPI.GPUXG, 0, "GPU XGM");
                            await Task.Delay(TimeSpan.FromSeconds(15));
                        }
                    }
                }
                else
                {

                    if (AppConfig.Is("xgm_special"))
                        Program.acpi.DeviceSet(AsusACPI.GPUXG, 0x101, "GPU XGM");
                    else
                        Program.acpi.DeviceSet(AsusACPI.GPUXG, 1, "GPU XGM");

                    InitXGM();
                    XGM.Light(AppConfig.Is("xmg_light"));

                    await Task.Delay(TimeSpan.FromSeconds(15));

                    if (AppConfig.IsMode("auto_apply"))
                        XGM.SetFan(AppConfig.GetFanConfig(AsusFan.XGM));

                    HardwareControl.RecreateGpuControl();

                }

                settings.Invoke(delegate
                {
                    InitGPUMode();
                });
            });
        }

        public void KillGPUApps()
        {
            if (HardwareControl.GpuControl is not null)
            {
                HardwareControl.GpuControl.KillGPUApps();
            }
        }

        // Manually forcing standard mode on shutdown/hibernate for some exotic cases
        // https://github.com/seerge/g-helper/pull/855 
        public void StandardModeFix()
        {
            if (!AppConfig.IsGPUFix()) return; // No config entry
            if (Program.acpi.DeviceGet(AsusACPI.GPUMux) == 0) return; // Ultimate mode

            Logger.WriteLine("Forcing Standard Mode on shutdown / hibernation");
            Program.acpi.SetGPUEco(0);
        }

    }
}
        uses: ﻿namespace GHelper.Gpu;

public  interface IGpuControl : IDisposable {
    bool IsNvidia { get; }
    bool IsValid { get; }
    public string FullName { get; }
    int? GetCurrentTemperature();
    int? GetGpuUse();
    void KillGPUApps();

}
        # Provide a unique ID to access the sarif output path
        id: connection.project.dir=
eclipse.preferences.version=1
        with: connection.project.dir=..
eclipse.preferences.version=1
          cmakeBuildDirectory: ${{ <manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.doctor_companion">
    <!-- Flutter needs it to communicate with the running application
         to allow setting breakpoints, to provide hot reload, etc.
    -->
    <uses-permission android:name="android.permission.INTERNET"/>
</manifest> }}
          # Ruleset file that will determine what checks will be run
          ruleset: package com.example.doctor_companion

import android.os.Bundle

import io.flutter.app.FlutterActivity
import io.flutter.plugins.GeneratedPluginRegistrant

class MainActivity: FlutterActivity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    GeneratedPluginRegistrant.registerWith(this)
  }
}

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: <manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.example">
    <!-- Flutter needs it to communicate with the running application
         to allow setting breakpoints, to provide hot reload, etc.
    -->
    <uses-permission android:name="android.permission.INTERNET"/>
</manifest>
        uses: package com.example.example

import io.flutter.embedding.android.FlutterActivity

class MainActivity: FlutterActivity() {
}
        with: <?xml version="1.0" encoding="utf-8"?>
<!-- Modify this file to customize your launch splash screen -->
<layer-list xmlns:android="http://schemas.android.com/apk/res/android">
    <item android:drawable="@android:color/white" />

    <!-- You can insert your own image assets here -->
    <!-- <item>
        <bitmap
            android:gravity="center"
            android:src="@mipmap/launch_image" />
    </item> -->
</layer-list>
          sarif_file: ${{ <manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.example.example">
    <!-- Flutter needs it to communicate with the running application
         to allow setting breakpoints, to provide hot reload, etc.
    -->
    <uses-permission android:name="android.permission.INTERNET"/>
</manifest> }}

      # Upload SARIF file as an Artifact to download and view
      # Default ignored files
/shelf/
/workspace.xml
      #   uses: <project version="4">
  <component name="ProjectRootManager">
    <output url="file://$PROJECT_DIR$/out" />
  </component>
</project>
      #   with: <?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="ProjectModuleManager">
    <modules>
      <module fileurl="file://$PROJECT_DIR$/.idea/vignettes.iml" filepath="$PROJECT_DIR$/.idea/vignettes.iml" />
    </modules>
  </component>
</project>
      #     name: <?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="VcsDirectoryMappings">
    <mapping directory="$PROJECT_DIR$/.." vcs="Git" />
  </component>
</project>
      #     path: ${{ <?xml version="1.0" encoding="UTF-8"?>
<module type="JAVA_MODULE" version="4">
  <component name="NewModuleRootManager" inherit-compiler-output="true">
    <exclude-output />
    <content url="file://$MODULE_DIR$">
      <excludeFolder url="file://$MODULE_DIR$/_shared/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/_shared/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/_shared/build" />
      <excludeFolder url="file://$MODULE_DIR$/basketball_ptr/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/basketball_ptr/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/basketball_ptr/build" />
      <excludeFolder url="file://$MODULE_DIR$/bubble_tab_bar/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/bubble_tab_bar/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/bubble_tab_bar/build" />
      <excludeFolder url="file://$MODULE_DIR$/constellations_list/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/constellations_list/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/constellations_list/build" />
      <excludeFolder url="file://$MODULE_DIR$/dark_ink_transition/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/dark_ink_transition/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/dark_ink_transition/build" />
      <excludeFolder url="file://$MODULE_DIR$/dog_slider/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/dog_slider/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/dog_slider/build" />
      <excludeFolder url="file://$MODULE_DIR$/drink_rewards_list/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/drink_rewards_list/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/drink_rewards_list/build" />
      <excludeFolder url="file://$MODULE_DIR$/fluid_nav_bar/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/fluid_nav_bar/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/fluid_nav_bar/build" />
      <excludeFolder url="file://$MODULE_DIR$/gooey_edge/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/gooey_edge/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/gooey_edge/build" />
      <excludeFolder url="file://$MODULE_DIR$/indie_3d/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/indie_3d/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/indie_3d/build" />
      <excludeFolder url="file://$MODULE_DIR$/parallax_travel_cards_hero/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/parallax_travel_cards_hero/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/parallax_travel_cards_hero/build" />
      <excludeFolder url="file://$MODULE_DIR$/parallax_travel_cards_list/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/parallax_travel_cards_list/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/parallax_travel_cards_list/build" />
      <excludeFolder url="file://$MODULE_DIR$/particle_swipe/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/particle_swipe/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/particle_swipe/build" />
      <excludeFolder url="file://$MODULE_DIR$/plant_forms/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/plant_forms/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/plant_forms/build" />
      <excludeFolder url="file://$MODULE_DIR$/product_detail_zoom/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/product_detail_zoom/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/product_detail_zoom/build" />
      <excludeFolder url="file://$MODULE_DIR$/sparkle_party/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/sparkle_party/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/sparkle_party/build" />
      <excludeFolder url="file://$MODULE_DIR$/spending_tracker/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/spending_tracker/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/spending_tracker/build" />
      <excludeFolder url="file://$MODULE_DIR$/ticket_fold/.dart_tool" />
      <excludeFolder url="file://$MODULE_DIR$/ticket_fold/.pub" />
      <excludeFolder url="file://$MODULE_DIR$/ticket_fold/build" />
    </content>
    <orderEntry type="inheritedJdk" />
    <orderEntry type="sourceFolder" forTests="false" />
  </component>
</module> }}
