# Sample workflow for building and deploying a Jekyll site to GitHub Pages
name: Deploy Jekyll with GitHub Pages dependencies preinstalled

on: #Wed Apr 10 15:27:10 PDT 2013
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.2.1-all.zip
  # Runs on pushes targeting the default branch
  push: <?xml version='1.0' encoding='utf-8'?>
<resources>
    <array name="qt_sources">
        <item>https://download.qt-project.org/ministro/android/qt5/qt-5.4</item>
    </array>

    <!-- The following is handled automatically by the deployment tool. It should
         not be edited manually. -->

    <array name="bundled_libs">
        <!-- %%INSERT_EXTRA_LIBS%% -->
    </array>

     <array name="qt_libs">
         <!-- %%INSERT_QT_LIBS%% -->
     </array>

    <array name="bundled_in_lib">
        <!-- %%INSERT_BUNDLED_IN_LIB%% -->
    </array>
    <array name="bundled_in_assets">
        <!-- %%INSERT_BUNDLED_IN_ASSETS%% -->
    </array>

</resources>
    branches: ["main"]

  # Allows you to run this workflow manually from the Actions tab
  workflow_dispatch: <?xml version="1.0" encoding="utf-8"?>
<resources>
    <!-- Allow anything connected -->
    <usb-device />
    <usb-accessory model="android.usbaoa" manufacturer="taisync" version="1.0"/>
</resources>


# Sets permissions of the GITHUB_TOKEN to allow deployment to GitHub Pages
permissions: package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * USB CDC/ACM serial driver implementation.
 *
 * @author mike wakerly (opensource@hoho.com)
 * @see <a
 *      href="http://www.usb.org/developers/devclass_docs/usbcdc11.pdf">Universal
 *      Serial Bus Class Definitions for Communication Devices, v1.1</a>
 */
public class CdcAcmSerialDriver extends CommonUsbSerialDriver {

    private final String TAG = CdcAcmSerialDriver.class.getSimpleName();

    private UsbInterface mControlInterface;
    private UsbInterface mDataInterface;

    private UsbEndpoint mControlEndpoint;
    private UsbEndpoint mReadEndpoint;
    private UsbEndpoint mWriteEndpoint;

    private boolean mRts = false;
    private boolean mDtr = false;

    private static final int USB_RECIP_INTERFACE = 0x01;
    private static final int USB_RT_ACM = UsbConstants.USB_TYPE_CLASS | USB_RECIP_INTERFACE;

    private static final int SET_LINE_CODING = 0x20;  // USB CDC 1.1 section 6.2
    private static final int GET_LINE_CODING = 0x21;
    private static final int SET_CONTROL_LINE_STATE = 0x22;
    private static final int SEND_BREAK = 0x23;

    public CdcAcmSerialDriver(UsbDevice device) {
        super(device);
    }

    @Override
    public void open() throws IOException {
        Log.d(TAG, "device " + mDevice);
        mControlInterface = null;
        mDataInterface = null;
        mWriteEndpoint = null;
        mReadEndpoint = null;

        // locate all needed interfaces
        for(int i = 0; i < mDevice.getInterfaceCount();i++){
            UsbInterface iface = mDevice.getInterface(i);

            switch(iface.getInterfaceClass()){
                case UsbConstants.USB_CLASS_COMM:
                    mControlInterface = iface;
                    Log.d(TAG, "control iface=" + iface);
                    break;
                case UsbConstants.USB_CLASS_CDC_DATA:
                    mDataInterface = iface;
                    Log.d(TAG, "data iface=" + iface);
                    break;
                default:
                    Log.d(TAG, "skipping iface=" + iface);
                    break;
            }
        }

        // failback to the old way
        if(mControlInterface == null) {
            mControlInterface = mDevice.getInterface(0);
            Log.d(TAG, "Failback: Control iface=" + mControlInterface);
        }

        if (!mConnection.claimInterface(mControlInterface, true)) {
            throw new IOException("Could not claim control interface.");
        }

        mControlEndpoint = mControlInterface.getEndpoint(0);
        Log.d(TAG, "Control endpoint: " + mControlEndpoint);


        if(mDataInterface == null) {
            mDataInterface = mDevice.getInterface(1);
            Log.d(TAG, "Failback: data iface=" + mDataInterface);
        }

        if (!mConnection.claimInterface(mDataInterface, true)) {
            throw new IOException("Could not claim data interface.");
        }

        for(int i = 0; i < mDataInterface.getEndpointCount(); i++) {
            UsbEndpoint endpoint = mDataInterface.getEndpoint(i);
            switch (endpoint.getDirection()) {
                case UsbConstants.USB_DIR_OUT:
                    mWriteEndpoint = endpoint;
                    Log.d(TAG, "Write endpoint: " + mWriteEndpoint);
                    break;
                case UsbConstants.USB_DIR_IN:
                    mReadEndpoint = endpoint;
                    Log.d(TAG, "Read endpoint: " + mReadEndpoint);
                    break;
            }
        }

        if(mReadEndpoint == null || mWriteEndpoint == null){
            // failback to the old method
            mReadEndpoint = mDataInterface.getEndpoint(0);
            Log.d(TAG, "Read endpoint direction: " + mReadEndpoint.getDirection());
            mWriteEndpoint = mDataInterface.getEndpoint(1);
            Log.d(TAG, "Write endpoint direction: " + mWriteEndpoint.getDirection());
        }
    }

    private int sendAcmControlMessage(int request, int value, byte[] buf) {
        return mConnection.controlTransfer(
                USB_RT_ACM, request, value, 0, buf, buf != null ? buf.length : 0, 5000);
    }

    @Override
    public void close() throws IOException {
        mConnection.close();
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        final int numBytesRead;
        synchronized (mReadBufferLock) {
            int readAmt = Math.min(dest.length, mReadBuffer.length);
            numBytesRead = mConnection.bulkTransfer(mReadEndpoint, mReadBuffer, readAmt,
                    timeoutMillis);
            if (numBytesRead < 0) {
                // This sucks: we get -1 on timeout, not 0 as preferred.
                // We *should* use UsbRequest, except it has a bug/api oversight
                // where there is no way to determine the number of bytes read
                // in response :\ -- http://b.android.com/28023
                return 0;
            }
            System.arraycopy(mReadBuffer, 0, dest, 0, numBytesRead);
        }
        return numBytesRead;
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        // TODO(mikey): Nearly identical to FtdiSerial write. Refactor.
        int offset = 0;

        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;

            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;

                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }

                amtWritten = mConnection.bulkTransfer(mWriteEndpoint, writeBuffer, writeLength,
                        timeoutMillis);
            }
            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length=" + src.length);
            }

            //Log.d(TAG, "Wrote amt=" + amtWritten + " attempted=" + writeLength);
            offset += amtWritten;
        }
        return offset;
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity) {
        byte stopBitsByte;
        switch (stopBits) {
            case STOPBITS_1: stopBitsByte = 0; break;
            case STOPBITS_1_5: stopBitsByte = 1; break;
            case STOPBITS_2: stopBitsByte = 2; break;
            default: throw new IllegalArgumentException("Bad value for stopBits: " + stopBits);
        }

        byte parityBitesByte;
        switch (parity) {
            case PARITY_NONE: parityBitesByte = 0; break;
            case PARITY_ODD: parityBitesByte = 1; break;
            case PARITY_EVEN: parityBitesByte = 2; break;
            case PARITY_MARK: parityBitesByte = 3; break;
            case PARITY_SPACE: parityBitesByte = 4; break;
            default: throw new IllegalArgumentException("Bad value for parity: " + parity);
        }

        byte[] msg = {
                (byte) ( baudRate & 0xff),
                (byte) ((baudRate >> 8 ) & 0xff),
                (byte) ((baudRate >> 16) & 0xff),
                (byte) ((baudRate >> 24) & 0xff),
                stopBitsByte,
                parityBitesByte,
                (byte) dataBits};
        sendAcmControlMessage(SET_LINE_CODING, 0, msg);
    }

    @Override
    public boolean getCD() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getCTS() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getDSR() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getDTR() throws IOException {
        return mDtr;
    }

    @Override
    public void setDTR(boolean value) throws IOException {
        mDtr = value;
        setDtrRts();
    }

    @Override
    public boolean getRI() throws IOException {
        return false;  // TODO
    }

    @Override
    public boolean getRTS() throws IOException {
        return mRts;
    }

    @Override
    public void setRTS(boolean value) throws IOException {
        mRts = value;
        setDtrRts();
    }

    private void setDtrRts() {
        int value = (mRts ? 0x2 : 0) | (mDtr ? 0x1 : 0);
        sendAcmControlMessage(SET_CONTROL_LINE_STATE, value, null);
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ARDUINO),
                new int[] {
                        UsbId.ARDUINO_UNO,
                        UsbId.ARDUINO_UNO_R3,
                        UsbId.ARDUINO_MEGA_2560,
                        UsbId.ARDUINO_MEGA_2560_R3,
                        UsbId.ARDUINO_SERIAL_ADAPTER,
                        UsbId.ARDUINO_SERIAL_ADAPTER_R3,
                        UsbId.ARDUINO_MEGA_ADK,
                        UsbId.ARDUINO_MEGA_ADK_R3,
                        UsbId.ARDUINO_LEONARDO,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_VAN_OOIJEN_TECH),
                new int[] {
                    UsbId.VAN_OOIJEN_TECH_TEENSYDUINO_SERIAL,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ATMEL),
                new int[] {
                    UsbId.ATMEL_LUFA_CDC_DEMO_APP,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_LEAFLABS),
                new int[] {
                    UsbId.LEAFLABS_MAPLE,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_PX4),
                new int[] {
                    UsbId.DEVICE_PX4FMU,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_UBLOX),
                new int[] {
                    UsbId.DEVICE_UBLOX_5,
                    UsbId.DEVICE_UBLOX_6,
                    UsbId.DEVICE_UBLOX_7,
                    UsbId.DEVICE_UBLOX_8,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_OPENPILOT),
                new int[] {
                    UsbId.DEVICE_CC3D,
                    UsbId.DEVICE_REVOLUTION,
                    UsbId.DEVICE_SPARKY2,
                    UsbId.DEVICE_OPLINK,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ARDUPILOT_CHIBIOS1),
                new int[] {
                    UsbId.DEVICE_ARDUPILOT_CHIBIOS,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_ARDUPILOT_CHIBIOS2),
                new int[] {
                    UsbId.DEVICE_ARDUPILOT_CHIBIOS,
                });
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_DRAGONLINK),
                new int[] {
                    UsbId.DEVICE_DRAGONLINK,
                });
        return supportedDevices;
    }

}
  contents: /* Copyright 2013 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */

package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

import java.io.IOException;

/**
 * A base class shared by several driver implementations.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
abstract class CommonUsbSerialDriver implements UsbSerialDriver {

    public static final int DEFAULT_READ_BUFFER_SIZE = 16 * 1024;
    public static final int DEFAULT_WRITE_BUFFER_SIZE = 16 * 1024;

    protected final UsbDevice   mDevice;
    protected final Object      mReadBufferLock = new Object();
    protected final Object      mWriteBufferLock = new Object();

    protected UsbDeviceConnection mConnection = null;

    private int _permissionStatus = permissionStatusRequestRequired;

    /** Internal read buffer.  Guarded by {@link #mReadBufferLock}. */
    protected byte[] mReadBuffer;

    /** Internal write buffer.  Guarded by {@link #mWriteBufferLock}. */
    protected byte[] mWriteBuffer;

    public CommonUsbSerialDriver(UsbDevice device) {
        mDevice = device;

        mReadBuffer = new byte[DEFAULT_READ_BUFFER_SIZE];
        mWriteBuffer = new byte[DEFAULT_WRITE_BUFFER_SIZE];
    }

    @Override
    public void setConnection(UsbDeviceConnection connection) {
        mConnection = connection;
    }

    @Override
    public int permissionStatus() {
        return _permissionStatus;
    }

    @Override
    public void setPermissionStatus(int permissionStatus) {
        _permissionStatus = permissionStatus;
    }

    /**
     * Returns the currently-bound USB device.
     *
     * @return the device
     */
    @Override
    public final UsbDevice getDevice() {
        return mDevice;
    }

    /**
     * Returns the currently-bound USB device connection.
     *
     * @return the device connection
     */
    @Override
    public final UsbDeviceConnection getDeviceConnection() {
        return mConnection;
    }



    /**
     * Sets the size of the internal buffer used to exchange data with the USB
     * stack for read operations.  Most users should not need to change this.
     *
     * @param bufferSize the size in bytes
     */
    public final void setReadBufferSize(int bufferSize) {
        synchronized (mReadBufferLock) {
            if (bufferSize == mReadBuffer.length) {
                return;
            }
            mReadBuffer = new byte[bufferSize];
        }
    }

    /**
     * Sets the size of the internal buffer used to exchange data with the USB
     * stack for write operations.  Most users should not need to change this.
     *
     * @param bufferSize the size in bytes
     */
    public final void setWriteBufferSize(int bufferSize) {
        synchronized (mWriteBufferLock) {
            if (bufferSize == mWriteBuffer.length) {
                return;
            }
            mWriteBuffer = new byte[bufferSize];
        }
    }

    @Override
    public abstract void open() throws IOException;

    @Override
    public abstract void close() throws IOException;

    @Override
    public abstract int read(final byte[] dest, final int timeoutMillis) throws IOException;

    @Override
    public abstract int write(final byte[] src, final int timeoutMillis) throws IOException;

    @Override
    public abstract void setParameters(
            int baudRate, int dataBits, int stopBits, int parity) throws IOException;

    @Override
    public abstract boolean getCD() throws IOException;

    @Override
    public abstract boolean getCTS() throws IOException;

    @Override
    public abstract boolean getDSR() throws IOException;

    @Override
    public abstract boolean getDTR() throws IOException;

    @Override
    public abstract void setDTR(boolean value) throws IOException;

    @Override
    public abstract boolean getRI() throws IOException;

    @Override
    public abstract boolean getRTS() throws IOException;

    @Override
    public abstract void setRTS(boolean value) throws IOException;

    @Override
    public boolean purgeHwBuffers(boolean flushReadBuffers, boolean flushWriteBuffers) throws IOException {
        return !flushReadBuffers && !flushWriteBuffers;
    }

}
  pages: package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

public class Cp2102SerialDriver extends CommonUsbSerialDriver {

    private static final String TAG = Cp2102SerialDriver.class.getSimpleName();

    private static final int DEFAULT_BAUD_RATE = 9600;

    private static final int USB_WRITE_TIMEOUT_MILLIS = 5000;

    /*
     * Configuration Request Types
     */
    private static final int REQTYPE_HOST_TO_DEVICE = 0x41;

    /*
     * Configuration Request Codes
     */
    private static final int SILABSER_IFC_ENABLE_REQUEST_CODE = 0x00;
    private static final int SILABSER_SET_BAUDDIV_REQUEST_CODE = 0x01;
    private static final int SILABSER_SET_LINE_CTL_REQUEST_CODE = 0x03;
    private static final int SILABSER_SET_MHS_REQUEST_CODE = 0x07;
    private static final int SILABSER_SET_BAUDRATE = 0x1E;
    private static final int SILABSER_FLUSH_REQUEST_CODE = 0x12;

    private static final int FLUSH_READ_CODE = 0x0a;
    private static final int FLUSH_WRITE_CODE = 0x05;

    /*
     * SILABSER_IFC_ENABLE_REQUEST_CODE
     */
    private static final int UART_ENABLE = 0x0001;
    private static final int UART_DISABLE = 0x0000;

    /*
     * SILABSER_SET_BAUDDIV_REQUEST_CODE
     */
    private static final int BAUD_RATE_GEN_FREQ = 0x384000;

    /*
     * SILABSER_SET_MHS_REQUEST_CODE
     */
    private static final int MCR_DTR = 0x0001;
    private static final int MCR_RTS = 0x0002;
    private static final int MCR_ALL = 0x0003;

    private static final int CONTROL_WRITE_DTR = 0x0100;
    private static final int CONTROL_WRITE_RTS = 0x0200;    

    private UsbEndpoint mReadEndpoint;
    private UsbEndpoint mWriteEndpoint; 

    public Cp2102SerialDriver(UsbDevice device) {
        super(device);
    }

    private int setConfigSingle(int request, int value) {
        return mConnection.controlTransfer(REQTYPE_HOST_TO_DEVICE, request, value, 
                0, null, 0, USB_WRITE_TIMEOUT_MILLIS);
    }

    @Override
    public void open() throws IOException {        
        boolean opened = false;
        try {
            for (int i = 0; i < mDevice.getInterfaceCount(); i++) {                
                UsbInterface usbIface = mDevice.getInterface(i);
                if (mConnection.claimInterface(usbIface, true)) {
                    Log.d(TAG, "claimInterface " + i + " SUCCESS");                    
                } else {
                    Log.d(TAG, "claimInterface " + i + " FAIL");
                }
            }                       

            UsbInterface dataIface = mDevice.getInterface(mDevice.getInterfaceCount() - 1);
            for (int i = 0; i < dataIface.getEndpointCount(); i++) {
                UsbEndpoint ep = dataIface.getEndpoint(i);
                if (ep.getType() == UsbConstants.USB_ENDPOINT_XFER_BULK) {
                    if (ep.getDirection() == UsbConstants.USB_DIR_IN) {
                        mReadEndpoint = ep;
                    } else {
                        mWriteEndpoint = ep;
                    }
                }
            }

            setConfigSingle(SILABSER_IFC_ENABLE_REQUEST_CODE, UART_ENABLE);
            setConfigSingle(SILABSER_SET_MHS_REQUEST_CODE, MCR_ALL | CONTROL_WRITE_DTR | CONTROL_WRITE_RTS);
            setConfigSingle(SILABSER_SET_BAUDDIV_REQUEST_CODE, BAUD_RATE_GEN_FREQ / DEFAULT_BAUD_RATE);            
//            setParameters(DEFAULT_BAUD_RATE, DEFAULT_DATA_BITS, DEFAULT_STOP_BITS, DEFAULT_PARITY);
            opened = true;
        } finally {
            if (!opened) {
                close();
            }
        }        
    }

    @Override
    public void close() throws IOException {
        setConfigSingle(SILABSER_IFC_ENABLE_REQUEST_CODE, UART_DISABLE);
        mConnection.close();
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        final int numBytesRead;
        synchronized (mReadBufferLock) {
            int readAmt = Math.min(dest.length, mReadBuffer.length);
            numBytesRead = mConnection.bulkTransfer(mReadEndpoint, mReadBuffer, readAmt,
                    timeoutMillis);
            if (numBytesRead < 0) {
                // This sucks: we get -1 on timeout, not 0 as preferred.
                // We *should* use UsbRequest, except it has a bug/api oversight
                // where there is no way to determine the number of bytes read
                // in response :\ -- http://b.android.com/28023
                return 0;
            }
            System.arraycopy(mReadBuffer, 0, dest, 0, numBytesRead);
        }
        return numBytesRead;
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        int offset = 0;

        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;

            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;

                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }

                amtWritten = mConnection.bulkTransfer(mWriteEndpoint, writeBuffer, writeLength,
                        timeoutMillis);
            }
            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length=" + src.length);
            }

            //Log.d(TAG, "Wrote amt=" + amtWritten + " attempted=" + writeLength);
            offset += amtWritten;
        }
        return offset;
    }

    private void setBaudRate(int baudRate) throws IOException {   
        byte[] data = new byte[] {
                (byte) ( baudRate & 0xff),
                (byte) ((baudRate >> 8 ) & 0xff),
                (byte) ((baudRate >> 16) & 0xff),
                (byte) ((baudRate >> 24) & 0xff)
        };
        int ret = mConnection.controlTransfer(REQTYPE_HOST_TO_DEVICE, SILABSER_SET_BAUDRATE, 
                0, 0, data, 4, USB_WRITE_TIMEOUT_MILLIS);
        if (ret < 0) {
            throw new IOException("Error setting baud rate.");
        }
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity)
            throws IOException {
        setBaudRate(baudRate);

        int configDataBits = 0;
        switch (dataBits) {
            case DATABITS_5:
                configDataBits |= 0x0500;
                break;
            case DATABITS_6:
                configDataBits |= 0x0600;
                break;
            case DATABITS_7:
                configDataBits |= 0x0700;
                break;
            case DATABITS_8:
                configDataBits |= 0x0800;
                break;
            default:
                configDataBits |= 0x0800;
                break;
        }
        setConfigSingle(SILABSER_SET_LINE_CTL_REQUEST_CODE, configDataBits);

        int configParityBits = 0; // PARITY_NONE
        switch (parity) {
            case PARITY_ODD:
                configParityBits |= 0x0010;
                break;
            case PARITY_EVEN:
                configParityBits |= 0x0020;
                break;            
        }
        setConfigSingle(SILABSER_SET_LINE_CTL_REQUEST_CODE, configParityBits);

        int configStopBits = 0;
        switch (stopBits) {
            case STOPBITS_1:
                configStopBits |= 0;
                break;
            case STOPBITS_2:
                configStopBits |= 2;
                break;
        }
        setConfigSingle(SILABSER_SET_LINE_CTL_REQUEST_CODE, configStopBits);        
    }

    @Override
    public boolean getCD() throws IOException {
        return false;
    }

    @Override
    public boolean getCTS() throws IOException {
        return false;
    }

    @Override
    public boolean getDSR() throws IOException {
        return false;
    }

    @Override
    public boolean getDTR() throws IOException {
        return true;
    }

    @Override
    public void setDTR(boolean value) throws IOException {
    }

    @Override
    public boolean getRI() throws IOException {
        return false;
    }

    @Override
    public boolean getRTS() throws IOException {
        return true;
    }

    @Override
    public boolean purgeHwBuffers(boolean purgeReadBuffers,
            boolean purgeWriteBuffers) throws IOException {
        int value = (purgeReadBuffers ? FLUSH_READ_CODE : 0)
                | (purgeWriteBuffers ? FLUSH_WRITE_CODE : 0);

        if (value != 0) {
            setConfigSingle(SILABSER_FLUSH_REQUEST_CODE, value);
        }

        return true;
    }

    @Override
    public void setRTS(boolean value) throws IOException {
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_SILAB),
                new int[] {
                        UsbId.SILAB_CP2102
                });
        return supportedDevices;
    }


}
  id-token: /* Copyright 2011 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */

// IMPORTANT NOTE:
//  This code has been modified from the original source. It now uses the FTDI driver provided by
//  ftdichip.com to communicate with an FTDI device. The previous code did not work with all FTDI
//  devices.
package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbRequest;
import android.util.Log;

import com.ftdi.j2xx.D2xxManager;
import com.ftdi.j2xx.FT_Device;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.LinkedHashMap;
import java.util.Map;

import org.mavlink.qgroundcontrol.QGCActivity;

/**
 * A {@link CommonUsbSerialDriver} implementation for a variety of FTDI devices
 * <p>
 * This driver is based on
 * <a href="http://www.intra2net.com/en/developer/libftdi">libftdi</a>, and is
 * copyright and subject to the following terms:
 *
 * <pre>
 *   Copyright (C) 2003 by Intra2net AG
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License
 *   version 2.1 as published by the Free Software Foundation;
 *
 *   opensource@intra2net.com
 *   http://www.intra2net.com/en/developer/libftdi
 * </pre>
 *
 * </p>
 * <p>
 * Some FTDI devices have not been tested; see later listing of supported and
 * unsupported devices. Devices listed as "supported" support the following
 * features:
 * <ul>
 * <li>Read and write of serial data (see {@link #read(byte[], int)} and
 * {@link #write(byte[], int)}.
 * <li>Setting baud rate (see {@link #setBaudRate(int)}).
 * </ul>
 * </p>
 * <p>
 * Supported and tested devices:
 * <ul>
 * <li>{@value DeviceType#TYPE_R}</li>
 * </ul>
 * </p>
 * <p>
 * Unsupported but possibly working devices (please contact the author with
 * feedback or patches):
 * <ul>
 * <li>{@value DeviceType#TYPE_2232C}</li>
 * <li>{@value DeviceType#TYPE_2232H}</li>
 * <li>{@value DeviceType#TYPE_4232H}</li>
 * <li>{@value DeviceType#TYPE_AM}</li>
 * <li>{@value DeviceType#TYPE_BM}</li>
 * </ul>
 * </p>
 *
 * @author mike wakerly (opensource@hoho.com)
 * @see <a href="http://code.google.com/p/usb-serial-for-android/">USB Serial
 * for Android project page</a>
 * @see <a href="http://www.ftdichip.com/">FTDI Homepage</a>
 * @see <a href="http://www.intra2net.com/en/developer/libftdi">libftdi</a>
 */
public class FtdiSerialDriver extends CommonUsbSerialDriver {

    public static final int USB_TYPE_STANDARD = 0x00 << 5;
    public static final int USB_TYPE_CLASS = 0x00 << 5;
    public static final int USB_TYPE_VENDOR = 0x00 << 5;
    public static final int USB_TYPE_RESERVED = 0x00 << 5;

    public static final int USB_RECIP_DEVICE = 0x00;
    public static final int USB_RECIP_INTERFACE = 0x01;
    public static final int USB_RECIP_ENDPOINT = 0x02;
    public static final int USB_RECIP_OTHER = 0x03;

    public static final int USB_ENDPOINT_IN = 0x80;
    public static final int USB_ENDPOINT_OUT = 0x00;

    public static final int USB_WRITE_TIMEOUT_MILLIS = 5000;
    public static final int USB_READ_TIMEOUT_MILLIS = 5000;

    // From ftdi.h
    /**
     * Reset the port.
     */
    private static final int SIO_RESET_REQUEST = 0;

    /**
     * Set the modem control register.
     */
    private static final int SIO_MODEM_CTRL_REQUEST = 1;

    /**
     * Set flow control register.
     */
    private static final int SIO_SET_FLOW_CTRL_REQUEST = 2;

    /**
     * Set baud rate.
     */
    private static final int SIO_SET_BAUD_RATE_REQUEST = 3;

    /**
     * Set the data characteristics of the port.
     */
    private static final int SIO_SET_DATA_REQUEST = 4;

    private static final int SIO_RESET_SIO = 0;
    private static final int SIO_RESET_PURGE_RX = 1;
    private static final int SIO_RESET_PURGE_TX = 2;

    public static final int FTDI_DEVICE_OUT_REQTYPE =
            UsbConstants.USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT;

    public static final int FTDI_DEVICE_IN_REQTYPE =
            UsbConstants.USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN;

    /**
     * Length of the modem status header, transmitted with every read.
     */
    private static final int MODEM_STATUS_HEADER_LENGTH = 2;

    private final String TAG = FtdiSerialDriver.class.getSimpleName();

    private DeviceType mType;

    /**
     * FTDI chip types.
     */
    private static enum DeviceType {
        TYPE_BM, TYPE_AM, TYPE_2232C, TYPE_R, TYPE_2232H, TYPE_4232H;
    }

    private int mInterface = 0; /* INTERFACE_ANY */

    private int mMaxPacketSize = 64; // TODO(mikey): detect

    /**
     * Due to http://b.android.com/28023 , we cannot use UsbRequest async reads
     * since it gives no indication of number of bytes read. Set this to
     * {@code true} on platforms where it is fixed.
     */
    private static final boolean ENABLE_ASYNC_READS = false;

    FT_Device m_ftDev;

    /**
     * Filter FTDI status bytes from buffer
     * @param src The source buffer (which contains status bytes)
     * @param dest The destination buffer to write the status bytes into (can be src)
     * @param totalBytesRead Number of bytes read to src
     * @param maxPacketSize The USB endpoint max packet size
     * @return The number of payload bytes
     */
    private final int filterStatusBytes(byte[] src, byte[] dest, int totalBytesRead, int maxPacketSize) {
        final int packetsCount = totalBytesRead / maxPacketSize + 1;
        for (int packetIdx = 0; packetIdx < packetsCount; ++packetIdx) {
            final int count = (packetIdx == (packetsCount - 1))
                    ? (totalBytesRead % maxPacketSize) - MODEM_STATUS_HEADER_LENGTH
                    : maxPacketSize - MODEM_STATUS_HEADER_LENGTH;
            if (count > 0) {
                System.arraycopy(src,
                        packetIdx * maxPacketSize + MODEM_STATUS_HEADER_LENGTH,
                        dest,
                        packetIdx * (maxPacketSize - MODEM_STATUS_HEADER_LENGTH),
                        count);
            }
        }

      return totalBytesRead - (packetsCount * 2);
    }

    /**
     * Constructor.
     *
     * @param usbDevice the {@link UsbDevice} to use
     * @param usbConnection the {@link UsbDeviceConnection} to use
     * @throws UsbSerialRuntimeException if the given device is incompatible
     *             with this driver
     */
    public FtdiSerialDriver(UsbDevice usbDevice) {
        super(usbDevice);
        mType = null;
    }

    public void reset() throws IOException {
        int result = mConnection.controlTransfer(FTDI_DEVICE_OUT_REQTYPE, SIO_RESET_REQUEST,
                SIO_RESET_SIO, 0 /* index */, null, 0, USB_WRITE_TIMEOUT_MILLIS);
        if (result != 0) {
            throw new IOException("Reset failed: result=" + result);
        }

        // TODO(mikey): autodetect.
        mType = DeviceType.TYPE_R;
    }

    @Override
    public void open() throws IOException {
        D2xxManager ftD2xx = null;
        try {
            ftD2xx = D2xxManager.getInstance(QGCActivity.m_context);
        } catch (D2xxManager.D2xxException ex) {
            QGCActivity.qgcLogDebug("D2xxManager.getInstance threw exception: " + ex.getMessage());
        }

        if (ftD2xx == null) {
            String errMsg = "Unable to retrieve D2xxManager instance.";
            QGCActivity.qgcLogWarning(errMsg);
            throw new IOException(errMsg);
        }
        QGCActivity.qgcLogDebug("Opened D2xxManager");

        int DevCount = ftD2xx.createDeviceInfoList(QGCActivity.m_context);
        QGCActivity.qgcLogDebug("Found " + DevCount + " ftdi devices.");
        if (DevCount < 1) {
            throw new IOException("No FTDI Devices found");
        }

        m_ftDev = null;
        try {
            m_ftDev = ftD2xx.openByIndex(QGCActivity.m_context, 0);
        } catch (NullPointerException e) {
            QGCActivity.qgcLogDebug("ftD2xx.openByIndex exception: " + e.getMessage());
        } finally {
            if (m_ftDev == null) {
                throw new IOException("No FTDI Devices found");
            }
        }
        QGCActivity.qgcLogDebug("Opened FTDI device.");
    }

    @Override
    public void close() {
        if (m_ftDev != null) {
            try {
                m_ftDev.close();
            } catch (Exception e) {
                QGCActivity.qgcLogWarning("close exception: " + e.getMessage());
            }
            m_ftDev = null;
        }
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        int totalBytesRead = 0;
        int bytesAvailable = m_ftDev.getQueueStatus();

        if (bytesAvailable > 0) {
            bytesAvailable = Math.min(4096, bytesAvailable);
            try {
                totalBytesRead = m_ftDev.read(dest, bytesAvailable, timeoutMillis);
            } catch (NullPointerException e) {
                final String errorMsg = "Error reading: " + e.getMessage();
                QGCActivity.qgcLogWarning(errorMsg);
                throw new IOException(errorMsg, e);
            }
        }

        return totalBytesRead;
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        try {
            m_ftDev.write(src);
            return src.length;
        } catch (Exception e) {
            QGCActivity.qgcLogWarning("Error writing: " + e.getMessage());
        }
        return 0;
    }

    private int setBaudRate(int baudRate) throws IOException {
        try {
            m_ftDev.setBaudRate(baudRate);
            return baudRate;
        } catch (Exception e) {
            QGCActivity.qgcLogWarning("Error setting baud rate: " + e.getMessage());
        }
        return 0;
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits, int parity) throws IOException {
        setBaudRate(baudRate);

        switch (dataBits) {
        case 7:
            dataBits = D2xxManager.FT_DATA_BITS_7;
            break;
        case 8:
        default:
            dataBits = D2xxManager.FT_DATA_BITS_8;
            break;
        }

        switch (stopBits) {
        default:
        case 0:
            stopBits = D2xxManager.FT_STOP_BITS_1;
            break;
        case 1:
            stopBits = D2xxManager.FT_STOP_BITS_2;
            break;
        }

        switch (parity) {
        default:
        case 0:
            parity = D2xxManager.FT_PARITY_NONE;
            break;
        case 1:
            parity = D2xxManager.FT_PARITY_ODD;
            break;
        case 2:
            parity = D2xxManager.FT_PARITY_EVEN;
            break;
        case 3:
            parity = D2xxManager.FT_PARITY_MARK;
            break;
        case 4:
            parity = D2xxManager.FT_PARITY_SPACE;
            break;
        }

        try {
            m_ftDev.setDataCharacteristics((byte)dataBits, (byte)stopBits, (byte)parity);
        } catch (Exception e) {
            QGCActivity.qgcLogWarning("Error setDataCharacteristics: " + e.getMessage());
        }
    }
    @Override
    public boolean getCD() throws IOException {
        return false;
    }

    @Override
    public boolean getCTS() throws IOException {
        return false;
    }

    @Override
    public boolean getDSR() throws IOException {
        return false;
    }

    @Override
    public boolean getDTR() throws IOException {
        return false;
    }

    @Override
    public void setDTR(boolean value) throws IOException {
    }

    @Override
    public boolean getRI() throws IOException {
        return false;
    }

    @Override
    public boolean getRTS() throws IOException {
        return false;
    }

    @Override
    public void setRTS(boolean value) throws IOException {
    }

    @Override
    public boolean purgeHwBuffers(boolean purgeReadBuffers, boolean purgeWriteBuffers) throws IOException {
        if (purgeReadBuffers) {
            try {
                m_ftDev.purge(D2xxManager.FT_PURGE_RX);
            } catch (Exception e) {
                String errMsg = "Error purgeHwBuffers(RX): "+ e.getMessage();
                QGCActivity.qgcLogWarning(errMsg);
                throw new IOException(errMsg);
            }
        }

        if (purgeWriteBuffers) {
            try {
                m_ftDev.purge(D2xxManager.FT_PURGE_TX);
            } catch (Exception e) {
                String errMsg = "Error purgeHwBuffers(TX): " + e.getMessage();
                QGCActivity.qgcLogWarning(errMsg);
                throw new IOException(errMsg);
            }
        }

        return true;
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_FTDI),
                new int[] {
                    UsbId.FTDI_FT232R,
                    UsbId.FTDI_FT231X,                    
                });
        /*
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_PX4),
                new int[] {
                    UsbId.DEVICE_PX4FMU
                });
        */
        return supportedDevices;
    }

}

# Allow only one concurrent deployment, skipping runs queued between the run in-progress and latest queued.
# However, do NOT cancel in-progress runs as we want to allow these production deployments to complete.
concurrency: /* This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */

/*
 * Ported to usb-serial-for-android
 * by Felix Hädicke <felixhaedicke@web.de>
 *
 * Based on the pyprolific driver written
 * by Emmanuel Blot <emmanuel.blot@free.fr>
 * See https://github.com/eblot/pyftdi
 */

package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.LinkedHashMap;
import java.util.Map;

public class ProlificSerialDriver extends CommonUsbSerialDriver {
    private static final int USB_READ_TIMEOUT_MILLIS = 1000;
    private static final int USB_WRITE_TIMEOUT_MILLIS = 5000;

    private static final int USB_RECIP_INTERFACE = 0x01;

    private static final int PROLIFIC_VENDOR_READ_REQUEST = 0x01;
    private static final int PROLIFIC_VENDOR_WRITE_REQUEST = 0x01;

    private static final int PROLIFIC_VENDOR_OUT_REQTYPE = UsbConstants.USB_DIR_OUT
            | UsbConstants.USB_TYPE_VENDOR;

    private static final int PROLIFIC_VENDOR_IN_REQTYPE = UsbConstants.USB_DIR_IN
            | UsbConstants.USB_TYPE_VENDOR;

    private static final int PROLIFIC_CTRL_OUT_REQTYPE = UsbConstants.USB_DIR_OUT
            | UsbConstants.USB_TYPE_CLASS | USB_RECIP_INTERFACE;

    private static final int WRITE_ENDPOINT = 0x02;
    private static final int READ_ENDPOINT = 0x83;
    private static final int INTERRUPT_ENDPOINT = 0x81;

    private static final int FLUSH_RX_REQUEST = 0x08;
    private static final int FLUSH_TX_REQUEST = 0x09;

    private static final int SET_LINE_REQUEST = 0x20;
    private static final int SET_CONTROL_REQUEST = 0x22;

    private static final int CONTROL_DTR = 0x01;
    private static final int CONTROL_RTS = 0x02;

    private static final int STATUS_FLAG_CD = 0x01;
    private static final int STATUS_FLAG_DSR = 0x02;
    private static final int STATUS_FLAG_RI = 0x08;
    private static final int STATUS_FLAG_CTS = 0x80;

    private static final int STATUS_BUFFER_SIZE = 10;
    private static final int STATUS_BYTE_IDX = 8;

    private static final int DEVICE_TYPE_HX = 0;
    private static final int DEVICE_TYPE_0 = 1;
    private static final int DEVICE_TYPE_1 = 2;

    private int mDeviceType = DEVICE_TYPE_HX;

    private UsbEndpoint mReadEndpoint;
    private UsbEndpoint mWriteEndpoint;
    private UsbEndpoint mInterruptEndpoint;

    private int mControlLinesValue = 0;

    private int mBaudRate = -1, mDataBits = -1, mStopBits = -1, mParity = -1;

    private int mStatus = 0;
    private volatile Thread mReadStatusThread = null;
    private final Object mReadStatusThreadLock = new Object();
    boolean mStopReadStatusThread = false;
    private IOException mReadStatusException = null;

    private final String TAG = ProlificSerialDriver.class.getSimpleName();

    private final byte[] inControlTransfer(int requestType, int request,
            int value, int index, int length) throws IOException {
        byte[] buffer = new byte[length];
        int result = mConnection.controlTransfer(requestType, request, value,
                index, buffer, length, USB_READ_TIMEOUT_MILLIS);
        if (result != length) {
            throw new IOException(
                    String.format("ControlTransfer with value 0x%x failed: %d",
                            value, result));
        }
        return buffer;
    }

    private final void outControlTransfer(int requestType, int request,
            int value, int index, byte[] data) throws IOException {
        int length = (data == null) ? 0 : data.length;
        int result = mConnection.controlTransfer(requestType, request, value,
                index, data, length, USB_WRITE_TIMEOUT_MILLIS);
        if (result != length) {
            throw new IOException(
                    String.format("ControlTransfer with value 0x%x failed: %d",
                            value, result));
        }
    }

    private final byte[] vendorIn(int value, int index, int length)
            throws IOException {
        return inControlTransfer(PROLIFIC_VENDOR_IN_REQTYPE,
                PROLIFIC_VENDOR_READ_REQUEST, value, index, length);
    }

    private final void vendorOut(int value, int index, byte[] data)
            throws IOException {
        outControlTransfer(PROLIFIC_VENDOR_OUT_REQTYPE,
                PROLIFIC_VENDOR_WRITE_REQUEST, value, index, data);
    }

    private final void ctrlOut(int request, int value, int index, byte[] data)
            throws IOException {
        outControlTransfer(PROLIFIC_CTRL_OUT_REQTYPE, request, value, index,
                data);
    }

    private void doBlackMagic() throws IOException {
        vendorIn(0x8484, 0, 1);
        vendorOut(0x0404, 0, null);
        vendorIn(0x8484, 0, 1);
        vendorIn(0x8383, 0, 1);
        vendorIn(0x8484, 0, 1);
        vendorOut(0x0404, 1, null);
        vendorIn(0x8484, 0, 1);
        vendorIn(0x8383, 0, 1);
        vendorOut(0, 1, null);
        vendorOut(1, 0, null);
        vendorOut(2, (mDeviceType == DEVICE_TYPE_HX) ? 0x44 : 0x24, null);
    }

    private void resetDevice() throws IOException {
        purgeHwBuffers(true, true);
    }

    private void setControlLines(int newControlLinesValue) throws IOException {
        ctrlOut(SET_CONTROL_REQUEST, newControlLinesValue, 0, null);
        mControlLinesValue = newControlLinesValue;
    }

    private final void readStatusThreadFunction() {
        try {
            while (!mStopReadStatusThread) {
                byte[] buffer = new byte[STATUS_BUFFER_SIZE];
                int readBytesCount = mConnection.bulkTransfer(mInterruptEndpoint,
                        buffer,
                        STATUS_BUFFER_SIZE,
                        500);
                if (readBytesCount > 0) {
                    if (readBytesCount == STATUS_BUFFER_SIZE) {
                        mStatus = buffer[STATUS_BYTE_IDX] & 0xff;
                    } else {
                        throw new IOException(
                                String.format("Invalid CTS / DSR / CD / RI status buffer received, expected %d bytes, but received %d",
                                        STATUS_BUFFER_SIZE,
                                        readBytesCount));
                    }
                }
            }
        } catch (IOException e) {
            mReadStatusException = e;
        }
    }

    private final int getStatus() throws IOException {
        if ((mReadStatusThread == null) && (mReadStatusException == null)) {
            synchronized (mReadStatusThreadLock) {
                if (mReadStatusThread == null) {
                    byte[] buffer = new byte[STATUS_BUFFER_SIZE];
                    int readBytes = mConnection.bulkTransfer(mInterruptEndpoint,
                            buffer,
                            STATUS_BUFFER_SIZE,
                            100);
                    if (readBytes != STATUS_BUFFER_SIZE) {
                        Log.w(TAG, "Could not read initial CTS / DSR / CD / RI status");
                    } else {
                        mStatus = buffer[STATUS_BYTE_IDX] & 0xff;
                    }

                    mReadStatusThread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            readStatusThreadFunction();
                        }
                    });
                    mReadStatusThread.setDaemon(true);
                    mReadStatusThread.start();
                }
            }
        }

        /* throw and clear an exception which occurred in the status read thread */
        IOException readStatusException = mReadStatusException;
        if (mReadStatusException != null) {
            mReadStatusException = null;
            throw readStatusException;
        }

        return mStatus;
    }

    private final boolean testStatusFlag(int flag) throws IOException {
        return ((getStatus() & flag) == flag);
    }

    public ProlificSerialDriver(UsbDevice device) {
        super(device);
    }

    @Override
    public void open() throws IOException {
        UsbInterface usbInterface = mDevice.getInterface(0);

        if (!mConnection.claimInterface(usbInterface, true)) {
            throw new IOException("Error claiming Prolific interface 0");
        }

        boolean openSuccessful = false;
        try {
            for (int i = 0; i < usbInterface.getEndpointCount(); ++i) {
                UsbEndpoint currentEndpoint = usbInterface.getEndpoint(i);

                switch (currentEndpoint.getAddress()) {
                case READ_ENDPOINT:
                    mReadEndpoint = currentEndpoint;
                    break;

                case WRITE_ENDPOINT:
                    mWriteEndpoint = currentEndpoint;
                    break;

                case INTERRUPT_ENDPOINT:
                    mInterruptEndpoint = currentEndpoint;
                    break;
                }
            }

            if (mDevice.getDeviceClass() == 0x02) {
                mDeviceType = DEVICE_TYPE_0;
            } else {
                try {
                    Method getRawDescriptorsMethod
                        = mConnection.getClass().getMethod("getRawDescriptors");
                    byte[] rawDescriptors
                        = (byte[]) getRawDescriptorsMethod.invoke(mConnection);
                    byte maxPacketSize0 = rawDescriptors[7];
                    if (maxPacketSize0 == 64) {
                        mDeviceType = DEVICE_TYPE_HX;
                    } else if ((mDevice.getDeviceClass() == 0x00)
                            || (mDevice.getDeviceClass() == 0xff)) {
                        mDeviceType = DEVICE_TYPE_1;
                    } else {
                      Log.w(TAG, "Could not detect PL2303 subtype, "
                          + "Assuming that it is a HX device");
                      mDeviceType = DEVICE_TYPE_HX;
                    }
                } catch (NoSuchMethodException e) {
                    Log.w(TAG, "Method UsbDeviceConnection.getRawDescriptors, "
                            + "required for PL2303 subtype detection, not "
                            + "available! Assuming that it is a HX device");
                    mDeviceType = DEVICE_TYPE_HX;
                } catch (Exception e) {
                    Log.e(TAG, "An unexpected exception occurred while trying "
                            + "to detect PL2303 subtype", e);
                }
            }

            setControlLines(mControlLinesValue);
            resetDevice();

            doBlackMagic();
            openSuccessful = true;
        } finally {
            if (!openSuccessful) {
              try {
                mConnection.releaseInterface(usbInterface);
              } catch (Exception ingored) {
                // Do not cover possible exceptions
              }
            }
        }
    }

    @Override
    public void close() throws IOException {
        try {
            mStopReadStatusThread = true;
            synchronized (mReadStatusThreadLock) {
                if (mReadStatusThread != null) {
                    try {
                        mReadStatusThread.join();
                    } catch (Exception e) {
                        Log.w(TAG, "An error occurred while waiting for status read thread", e);
                    }
                }
            }

            resetDevice();
        } finally {
            mConnection.releaseInterface(mDevice.getInterface(0));
        }
    }

    @Override
    public int read(byte[] dest, int timeoutMillis) throws IOException {
        synchronized (mReadBufferLock) {
            int readAmt = Math.min(dest.length, mReadBuffer.length);
            int numBytesRead = mConnection.bulkTransfer(mReadEndpoint, mReadBuffer,
                    readAmt, timeoutMillis);
            if (numBytesRead < 0) {
                return 0;
            }
            System.arraycopy(mReadBuffer, 0, dest, 0, numBytesRead);
            return numBytesRead;
        }
    }

    @Override
    public int write(byte[] src, int timeoutMillis) throws IOException {
        int offset = 0;

        while (offset < src.length) {
            final int writeLength;
            final int amtWritten;

            synchronized (mWriteBufferLock) {
                final byte[] writeBuffer;

                writeLength = Math.min(src.length - offset, mWriteBuffer.length);
                if (offset == 0) {
                    writeBuffer = src;
                } else {
                    // bulkTransfer does not support offsets, make a copy.
                    System.arraycopy(src, offset, mWriteBuffer, 0, writeLength);
                    writeBuffer = mWriteBuffer;
                }

                amtWritten = mConnection.bulkTransfer(mWriteEndpoint,
                        writeBuffer, writeLength, timeoutMillis);
            }

            if (amtWritten <= 0) {
                throw new IOException("Error writing " + writeLength
                        + " bytes at offset " + offset + " length="
                        + src.length);
            }

            offset += amtWritten;
        }
        return offset;
    }

    @Override
    public void setParameters(int baudRate, int dataBits, int stopBits,
            int parity) throws IOException {
        if ((mBaudRate == baudRate) && (mDataBits == dataBits)
                && (mStopBits == stopBits) && (mParity == parity)) {
            // Make sure no action is performed if there is nothing to change
            return;
        }

        byte[] lineRequestData = new byte[7];

        lineRequestData[0] = (byte) (baudRate & 0xff);
        lineRequestData[1] = (byte) ((baudRate >> 8) & 0xff);
        lineRequestData[2] = (byte) ((baudRate >> 16) & 0xff);
        lineRequestData[3] = (byte) ((baudRate >> 24) & 0xff);

        switch (stopBits) {
        case STOPBITS_1:
            lineRequestData[4] = 0;
            break;

        case STOPBITS_1_5:
            lineRequestData[4] = 1;
            break;

        case STOPBITS_2:
            lineRequestData[4] = 2;
            break;

        default:
            throw new IllegalArgumentException("Unknown stopBits value: " + stopBits);
        }

        switch (parity) {
        case PARITY_NONE:
            lineRequestData[5] = 0;
            break;

        case PARITY_ODD:
            lineRequestData[5] = 1;
            break;

        case PARITY_EVEN:
            lineRequestData[5] = 2;
            break;

        case PARITY_MARK:
            lineRequestData[5] = 3;
            break;

        case PARITY_SPACE:
            lineRequestData[5] = 4;
            break;

        default:
            throw new IllegalArgumentException("Unknown parity value: " + parity);
        }

        lineRequestData[6] = (byte) dataBits;

        ctrlOut(SET_LINE_REQUEST, 0, 0, lineRequestData);

        resetDevice();

        mBaudRate = baudRate;
        mDataBits = dataBits;
        mStopBits = stopBits;
        mParity = parity;
    }

    @Override
    public boolean getCD() throws IOException {
        return testStatusFlag(STATUS_FLAG_CD);
    }

    @Override
    public boolean getCTS() throws IOException {
        return testStatusFlag(STATUS_FLAG_CTS);
    }

    @Override
    public boolean getDSR() throws IOException {
        return testStatusFlag(STATUS_FLAG_DSR);
    }

    @Override
    public boolean getDTR() throws IOException {
        return ((mControlLinesValue & CONTROL_DTR) == CONTROL_DTR);
    }

    @Override
    public void setDTR(boolean value) throws IOException {
        int newControlLinesValue;
        if (value) {
            newControlLinesValue = mControlLinesValue | CONTROL_DTR;
        } else {
            newControlLinesValue = mControlLinesValue & ~CONTROL_DTR;
        }
        setControlLines(newControlLinesValue);
    }

    @Override
    public boolean getRI() throws IOException {
        return testStatusFlag(STATUS_FLAG_RI);
    }

    @Override
    public boolean getRTS() throws IOException {
        return ((mControlLinesValue & CONTROL_RTS) == CONTROL_RTS);
    }

    @Override
    public void setRTS(boolean value) throws IOException {
        int newControlLinesValue;
        if (value) {
            newControlLinesValue = mControlLinesValue | CONTROL_RTS;
        } else {
            newControlLinesValue = mControlLinesValue & ~CONTROL_RTS;
        }
        setControlLines(newControlLinesValue);
    }

    @Override
    public boolean purgeHwBuffers(boolean purgeReadBuffers, boolean purgeWriteBuffers) throws IOException {
        if (purgeReadBuffers) {
            vendorOut(FLUSH_RX_REQUEST, 0, null);
        }

        if (purgeWriteBuffers) {
            vendorOut(FLUSH_TX_REQUEST, 0, null);
        }

        return true;
    }

    public static Map<Integer, int[]> getSupportedDevices() {
        final Map<Integer, int[]> supportedDevices = new LinkedHashMap<Integer, int[]>();
        supportedDevices.put(Integer.valueOf(UsbId.VENDOR_PROLIFIC),
                new int[] { UsbId.PROLIFIC_PL2303, });
        return supportedDevices;
    }
}
  group: /* Copyright 2012 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */
package com.hoho.android.usbserial.driver;

/**
 * Registry of USB vendor/product ID constants.
 *
 * Culled from various sources; see
 * <a href="http://www.linux-usb.org/usb.ids">usb.ids</a> for one listing.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public final class UsbId {

    public static final int VENDOR_FTDI = 0x0403;
    public static final int FTDI_FT232R = 0x6001;
    public static final int FTDI_FT231X = 0x6015;

    public static final int VENDOR_PX4    = 0x26AC;
    public static final int DEVICE_PX4FMU = 0x11;

    public static final int VENDOR_ATMEL = 0x03EB;
    public static final int ATMEL_LUFA_CDC_DEMO_APP = 0x2044;

    public static final int VENDOR_ARDUINO = 0x2341;
    public static final int ARDUINO_UNO = 0x0001;
    public static final int ARDUINO_MEGA_2560 = 0x0010;
    public static final int ARDUINO_SERIAL_ADAPTER = 0x003b;
    public static final int ARDUINO_MEGA_ADK = 0x003f;
    public static final int ARDUINO_MEGA_2560_R3 = 0x0042;
    public static final int ARDUINO_UNO_R3 = 0x0043;
    public static final int ARDUINO_MEGA_ADK_R3 = 0x0044;
    public static final int ARDUINO_SERIAL_ADAPTER_R3 = 0x0044;
    public static final int ARDUINO_LEONARDO = 0x8036;

    public static final int VENDOR_VAN_OOIJEN_TECH = 0x16c0;
    public static final int VAN_OOIJEN_TECH_TEENSYDUINO_SERIAL = 0x0483;

    public static final int VENDOR_LEAFLABS = 0x1eaf;
    public static final int LEAFLABS_MAPLE = 0x0004;

    public static final int VENDOR_SILAB = 0x10c4;
    public static final int SILAB_CP2102 = 0xea60;

    public static final int VENDOR_PROLIFIC = 0x067b;
    public static final int PROLIFIC_PL2303 = 0x2303;

    public static final int VENDOR_UBLOX = 0x1546;
    public static final int DEVICE_UBLOX_5 = 0x01a5;
    public static final int DEVICE_UBLOX_6 = 0x01a6;
    public static final int DEVICE_UBLOX_7 = 0x01a7;
    public static final int DEVICE_UBLOX_8 = 0x01a8;

    public static final int VENDOR_OPENPILOT = 0x20A0;
    public static final int DEVICE_REVOLUTION = 0x415E;
    public static final int DEVICE_OPLINK = 0x415C;
    public static final int DEVICE_SPARKY2 = 0x41D0;
    public static final int DEVICE_CC3D = 0x415D;

    public static final int VENDOR_ARDUPILOT_CHIBIOS1 = 0x0483;
    public static final int VENDOR_ARDUPILOT_CHIBIOS2 = 0x1209;
    public static final int DEVICE_ARDUPILOT_CHIBIOS =  0x5740;

    public static final int VENDOR_DRAGONLINK = 0x1FC9;
    public static final int DEVICE_DRAGONLINK = 0x0083;

    private UsbId() {
        throw new IllegalAccessError("Non-instantiable class.");
    }

}
  cancel-in-progress: /* Copyright 2011 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */

package com.hoho.android.usbserial.driver;

import java.io.IOException;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

/**
 * Driver interface for a USB serial device.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public interface UsbSerialDriver {

    /** 5 data bits. */
    public static final int DATABITS_5 = 5;

    /** 6 data bits. */
    public static final int DATABITS_6 = 6;

    /** 7 data bits. */
    public static final int DATABITS_7 = 7;

    /** 8 data bits. */
    public static final int DATABITS_8 = 8;

    /** No flow control. */
    public static final int FLOWCONTROL_NONE = 0;

    /** RTS/CTS input flow control. */
    public static final int FLOWCONTROL_RTSCTS_IN = 1;

    /** RTS/CTS output flow control. */
    public static final int FLOWCONTROL_RTSCTS_OUT = 2;

    /** XON/XOFF input flow control. */
    public static final int FLOWCONTROL_XONXOFF_IN = 4;

    /** XON/XOFF output flow control. */
    public static final int FLOWCONTROL_XONXOFF_OUT = 8;

    /** No parity. */
    public static final int PARITY_NONE = 0;

    /** Odd parity. */
    public static final int PARITY_ODD = 1;

    /** Even parity. */
    public static final int PARITY_EVEN = 2;

    /** Mark parity. */
    public static final int PARITY_MARK = 3;

    /** Space parity. */
    public static final int PARITY_SPACE = 4;

    /** 1 stop bit. */
    public static final int STOPBITS_1 = 1;

    /** 1.5 stop bits. */
    public static final int STOPBITS_1_5 = 3;

    /** 2 stop bits. */
    public static final int STOPBITS_2 = 2;

    public static final int permissionStatusSuccess =           0;
    public static final int permissionStatusDenied =            1;
    public static final int permissionStatusRequested =         2;
    public static final int permissionStatusRequestRequired =   3;
    public static final int permissionStatusOpen =              4;
    public int permissionStatus();
    public void setPermissionStatus(int permissionStatus);

    public void setConnection(UsbDeviceConnection connection);

    /**
     * Returns the currently-bound USB device.
     *
     * @return the device
     */
    public UsbDevice getDevice();


    /**
     * Returns the currently-bound USB device.
     *
     * @return the device
     */
    public UsbDeviceConnection getDeviceConnection();

    /**
     * Opens and initializes the device as a USB serial device. Upon success,
     * caller must ensure that {@link #close()} is eventually called.
     *
     * @throws IOException on error opening or initializing the device.
     */
    public void open() throws IOException;

    /**
     * Closes the serial device.
     *
     * @throws IOException on error closing the device.
     */
    public void close() throws IOException;

    /**
     * Reads as many bytes as possible into the destination buffer.
     *
     * @param dest the destination byte buffer
     * @param timeoutMillis the timeout for reading
     * @return the actual number of bytes read
     * @throws IOException if an error occurred during reading
     */
    public int read(final byte[] dest, final int timeoutMillis) throws IOException;

    /**
     * Writes as many bytes as possible from the source buffer.
     *
     * @param src the source byte buffer
     * @param timeoutMillis the timeout for writing
     * @return the actual number of bytes written
     * @throws IOException if an error occurred during writing
     */
    public int write(final byte[] src, final int timeoutMillis) throws IOException;

    /**
     * Sets various serial port parameters.
     *
     * @param baudRate baud rate as an integer, for example {@code 115200}.
     * @param dataBits one of {@link #DATABITS_5}, {@link #DATABITS_6},
     *            {@link #DATABITS_7}, or {@link #DATABITS_8}.
     * @param stopBits one of {@link #STOPBITS_1}, {@link #STOPBITS_1_5}, or
     *            {@link #STOPBITS_2}.
     * @param parity one of {@link #PARITY_NONE}, {@link #PARITY_ODD},
     *            {@link #PARITY_EVEN}, {@link #PARITY_MARK}, or
     *            {@link #PARITY_SPACE}.
     * @throws IOException on error setting the port parameters
     */
    public void setParameters(
            int baudRate, int dataBits, int stopBits, int parity) throws IOException;

    /**
     * Gets the CD (Carrier Detect) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getCD() throws IOException;

    /**
     * Gets the CTS (Clear To Send) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getCTS() throws IOException;

    /**
     * Gets the DSR (Data Set Ready) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getDSR() throws IOException;

    /**
     * Gets the DTR (Data Terminal Ready) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getDTR() throws IOException;

    /**
     * Sets the DTR (Data Terminal Ready) bit on the underlying UART, if
     * supported.
     *
     * @param value the value to set
     * @throws IOException if an error occurred during writing
     */
    public void setDTR(boolean value) throws IOException;

    /**
     * Gets the RI (Ring Indicator) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getRI() throws IOException;

    /**
     * Gets the RTS (Request To Send) bit from the underlying UART.
     *
     * @return the current state, or {@code false} if not supported.
     * @throws IOException if an error occurred during reading
     */
    public boolean getRTS() throws IOException;

    /**
     * Sets the RTS (Request To Send) bit on the underlying UART, if
     * supported.
     *
     * @param value the value to set
     * @throws IOException if an error occurred during writing
     */
    public void setRTS(boolean value) throws IOException;

    /**
     * Flush non-transmitted output data and / or non-read input data
     * @param flushRX {@code true} to flush non-transmitted output data
     * @param flushTX {@code true} to flush non-read input data
     * @return {@code true} if the operation was successful, or
     * {@code false} if the operation is not supported by the driver or device
     * @throws IOException if an error occurred during flush
     */
    public boolean purgeHwBuffers(boolean flushRX, boolean flushTX) throws IOException;

}

jobs: /* Copyright 2011 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */

// IMPORTANT NOTE:
//  This source has been modified from the original such that testIfSupported only tests for a vendor id
//  match. If that matches it allows all product ids through. This provides for better match on unknown boards.

package com.hoho.android.usbserial.driver;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

/**
 * Helper class which finds compatible {@link UsbDevice}s and creates
 * {@link UsbSerialDriver} instances.
 *
 * <p/>
 * You don't need a Prober to use the rest of the library: it is perfectly
 * acceptable to instantiate driver instances manually. The Prober simply
 * provides convenience functions.
 *
 * <p/>
 * For most drivers, the corresponding {@link #probe(UsbManager, UsbDevice)}
 * method will either return an empty list (device unknown / unsupported) or a
 * singleton list. However, multi-port drivers may return multiple instances.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public enum UsbSerialProber {

    // TODO(mikey): Too much boilerplate.

    /**
     * Prober for {@link FtdiSerialDriver}.
     *
     * @see FtdiSerialDriver
     */
    FTDI_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, FtdiSerialDriver.getSupportedDevices())) {
                return Collections.emptyList();
            }
            final UsbSerialDriver driver = new FtdiSerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    },

    CDC_ACM_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(UsbManager manager, UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, CdcAcmSerialDriver.getSupportedDevices())) {
               return Collections.emptyList();
            }
            final UsbSerialDriver driver = new CdcAcmSerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    },

    SILAB_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, Cp2102SerialDriver.getSupportedDevices())) {
                return Collections.emptyList();
            }
            final UsbSerialDriver driver = new Cp2102SerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    },

    PROLIFIC_SERIAL {
        @Override
        public List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice) {
            if (!testIfSupported(usbDevice, ProlificSerialDriver.getSupportedDevices())) {
                return Collections.emptyList();
            }
            final UsbSerialDriver driver = new ProlificSerialDriver(usbDevice);
            return Collections.singletonList(driver);
        }
    };

    /**
     * Tests the supplied {@link UsbDevice} for compatibility with this enum
     * member, returning one or more driver instances if compatible.
     *
     * @param manager the {@link UsbManager} to use
     * @param usbDevice the raw {@link UsbDevice} to use
     * @return zero or more {@link UsbSerialDriver}, depending on compatibility
     *         (never {@code null}).
     */
    protected abstract List<UsbSerialDriver> probe(final UsbManager manager, final UsbDevice usbDevice);

    /**
     * Creates and returns a new {@link UsbSerialDriver} instance for the first
     * compatible {@link UsbDevice} found on the bus.  If none are found,
     * returns {@code null}.
     *
     * <p/>
     * The order of devices is undefined, therefore if there are multiple
     * devices on the bus, the chosen device may not be predictable (clients
     * should use {@link #findAllDevices(UsbManager)} instead).
     *
     * @param usbManager the {@link UsbManager} to use.
     * @return the first available {@link UsbSerialDriver}, or {@code null} if
     *         none are available.
     */
    public static UsbSerialDriver findFirstDevice(final UsbManager usbManager) {
        for (final UsbDevice usbDevice : usbManager.getDeviceList().values()) {
            for (final UsbSerialProber prober : values()) {
                final List<UsbSerialDriver> probedDevices = prober.probe(usbManager, usbDevice);
                if (!probedDevices.isEmpty()) {
                    return probedDevices.get(0);
                }
            }
        }
        return null;
    }

    /**
     * Creates a new {@link UsbSerialDriver} instance for all compatible
     * {@link UsbDevice}s found on the bus. If no compatible devices are found,
     * the list will be empty.
     *
     * @param usbManager
     * @return
     */
    public static List<UsbSerialDriver> findAllDevices(final UsbManager usbManager) {
        final List<UsbSerialDriver> result = new ArrayList<UsbSerialDriver>();
        //Log.i("QGC_UsbSerialProber", "Looking for USB devices");
        // For each UsbDevice, call probe() for each prober.
        for (final UsbDevice usbDevice : usbManager.getDeviceList().values()) {
            //Log.i("QGC_UsbSerialProber", "Probing device: " + usbDevice.getDeviceName() + " mid: " + usbDevice.getVendorId() + " pid: " + usbDevice.getDeviceId());
            result.addAll(probeSingleDevice(usbManager, usbDevice));
        }
        return result;
    }

    /**
     * Special method for testing a specific device for driver support,
     * returning any compatible driver(s).
     *
     * <p/>
     * Clients should ordinarily use {@link #findAllDevices(UsbManager)}, which
     * operates against the entire bus of devices. This method is useful when
     * testing against only a single target is desired.
     *
     * @param usbManager the {@link UsbManager} to use.
     * @param usbDevice the device to test against.
     * @return a list containing zero or more {@link UsbSerialDriver} instances.
     */
    public static List<UsbSerialDriver> probeSingleDevice(final UsbManager usbManager,
            UsbDevice usbDevice)
    {
        final List<UsbSerialDriver> result = new ArrayList<UsbSerialDriver>();
        for (final UsbSerialProber prober : values()) {
            final List<UsbSerialDriver> probedDevices = prober.probe(usbManager, usbDevice);
            result.addAll(probedDevices);
        }
        return result;
    }

    /**
     * Deprecated; Use {@link #findFirstDevice(UsbManager)}.
     *
     * @param usbManager
     * @return
     */
    @Deprecated
    public static UsbSerialDriver acquire(final UsbManager usbManager) {
        return findFirstDevice(usbManager);
    }

    /**
     * Deprecated; use {@link #probeSingleDevice(UsbManager, UsbDevice)}.
     *
     * @param usbManager
     * @param usbDevice
     * @return
     */
    @Deprecated
    public static UsbSerialDriver acquire(final UsbManager usbManager, final UsbDevice usbDevice) {
        final List<UsbSerialDriver> probedDevices = probeSingleDevice(usbManager, usbDevice);
        if (!probedDevices.isEmpty()) {
            return probedDevices.get(0);
        }
        return null;
    }

    /**
     * Returns {@code true} if the given device is found in the driver's
     * vendor/product map.
     *
     * @param usbDevice the device to test
     * @param supportedDevices map of vendor IDs to product ID(s)
     * @return {@code true} if supported
     */
    private static boolean testIfSupported(final UsbDevice usbDevice, final Map<Integer, int[]> supportedDevices) {
        return supportedDevices.containsKey(usbDevice.getVendorId());
    }
}
  # Build job
  build: /*
 * Copyright 2011 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

package com.hoho.android.usbserial.driver;

/**
 * Generic unchecked exception for the usbserial package.
 *
 * 
 */
@SuppressWarnings("serial")
public class UsbSerialRuntimeException extends RuntimeException {

    public UsbSerialRuntimeException() {
        super();
    }

    public UsbSerialRuntimeException(String detailMessage, Throwable throwable) {
        super(detailMessage, throwable);
    }

    public UsbSerialRuntimeException(String detailMessage) {
        super(detailMessage);
    }

    public UsbSerialRuntimeException(Throwable throwable) {
        super(throwable);
    }

}
    runs-on: /*
 * Copyright (C) 2012, Collabora Ltd.
 *   
 *
 * Copyright (C) 2015, Collabora Ltd.
 *   
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

package org.freedesktop.gstreamer.androidmedia;

import android.hardware.Camera;

public class GstAhcCallback implements Camera.PreviewCallback,
                                       Camera.ErrorCallback,
                                       Camera.AutoFocusCallback {
    public long mUserData;
    public long mCallback;

    public static native void gst_ah_camera_on_preview_frame(byte[] data, Camera camera,
                                                             long callback, long user_data);
    public static native void gst_ah_camera_on_error(int error, Camera camera,
                                                     long callback, long user_data);
    public static native void gst_ah_camera_on_auto_focus(boolean success, Camera camera,
                                                             long callback, long user_data);

    public GstAhcCallback(long callback, long user_data) {
        mCallback = callback;
        mUserData = user_data;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        gst_ah_camera_on_preview_frame(data, camera, mCallback, mUserData);
    }

    @Override
    public void onError(int error, Camera camera) {
        gst_ah_camera_on_error(error, camera, mCallback, mUserData);
    }

    @Override
    public void onAutoFocus(boolean success, Camera camera) {
        gst_ah_camera_on_auto_focus(success, camera, mCallback, mUserData);
    }
}
    steps: /*
 * Copyright (C) 2016 SurroundIO
 *   
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

package org.freedesktop.gstreamer.androidmedia;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

public class GstAhsCallback implements SensorEventListener {
    public long mUserData;
    public long mSensorCallback;
    public long mAccuracyCallback;

    public static native void gst_ah_sensor_on_sensor_changed(SensorEvent event,
                                                              long callback, long user_data);
    public static native void gst_ah_sensor_on_accuracy_changed(Sensor sensor, int accuracy,
                                                                long callback, long user_data);

    public GstAhsCallback(long sensor_callback,
        long accuracy_callback, long user_data) {
        mSensorCallback = sensor_callback;
        mAccuracyCallback = accuracy_callback;
        mUserData = user_data;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
      gst_ah_sensor_on_sensor_changed(event, mSensorCallback, mUserData);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
      gst_ah_sensor_on_accuracy_changed(sensor, accuracy,
          mAccuracyCallback, mUserData);
    }
}
      - name: /*
 * Copyright (C) 2015, Collabora Ltd.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

package org.freedesktop.gstreamer.androidmedia;

import android.graphics.SurfaceTexture;
import android.graphics.SurfaceTexture.OnFrameAvailableListener;

public class GstAmcOnFrameAvailableListener implements OnFrameAvailableListener
{
    private long context = 0;

    public synchronized void onFrameAvailable (SurfaceTexture surfaceTexture) {
        native_onFrameAvailable(context, surfaceTexture);
    }

    public synchronized long getContext () {
        return context;
    }

    public synchronized void setContext (long c) {
        context = c;
    }

    private native void native_onFrameAvailable (long context, SurfaceTexture surfaceTexture);
}
        uses: package org.mavlink.qgroundcontrol;

/* Copyright 2013 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */
///////////////////////////////////////////////////////////////////////////////////////////
//  Written by: Mike Goza April 2014
//
//  These routines interface with the Android USB Host devices for serial port communication.
//  The code uses the usb-serial-for-android software library.  The QGCActivity class is the
//  interface to the C++ routines through jni calls.  Do not change the functions without also
//  changing the corresponding calls in the C++ routines or you will break the interface.
//
////////////////////////////////////////////////////////////////////////////////////////////

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Timer;
import java.util.TimerTask;
import java.io.IOException;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.widget.Toast;
import android.util.Log;
import android.os.PowerManager;
import android.os.Bundle;
import android.app.PendingIntent;
import android.view.WindowManager;
import android.os.Bundle;
import android.bluetooth.BluetoothDevice;

import com.hoho.android.usbserial.driver.*;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;

public class QGCActivity extends QtActivity
{
    public  static int                                  BAD_DEVICE_ID = 0;
    private static QGCActivity                          _instance = null;
    private static UsbManager                           _usbManager = null;
    private static List<UsbSerialDriver>                _drivers;
    private static HashMap<Integer, UsbIoManager>       m_ioManager;
    private static HashMap<Integer, Long>               _userDataHashByDeviceId;
    private static final String                         TAG = "QGC_QGCActivity";
    private static PowerManager.WakeLock                _wakeLock;
    private static final String                         ACTION_USB_PERMISSION = "org.mavlink.qgroundcontrol.action.USB_PERMISSION";
    private static PendingIntent                        _usbPermissionIntent = null;
    private TaiSync                                     taiSync = null;
    private Timer                                       probeAccessoriesTimer = null;

    public static Context m_context;

    private final static ExecutorService m_Executor = Executors.newSingleThreadExecutor();

    private final static UsbIoManager.Listener m_Listener =
            new UsbIoManager.Listener()
            {
                @Override
                public void onRunError(Exception eA, long userData)
                {
                    Log.e(TAG, "onRunError Exception");
                    nativeDeviceException(userData, eA.getMessage());
                }

                @Override
                public void onNewData(final byte[] dataA, long userData)
                {
                    nativeDeviceNewData(userData, dataA);
                }
            };

    private final BroadcastReceiver mOpenAccessoryReceiver =
        new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (ACTION_USB_PERMISSION.equals(action)) {
                    UsbAccessory accessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (accessory != null && intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        openAccessory(accessory);
                    }
                } else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
                    UsbAccessory accessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (accessory != null) {
                        closeAccessory(accessory);
                    }
                }
            }
        };

    private static UsbSerialDriver _findDriverByDeviceId(int deviceId) {
        for (UsbSerialDriver driver: _drivers) {
            if (driver.getDevice().getDeviceId() == deviceId) {
                return driver;
            }
        }
        return null;
    }

    private static UsbSerialDriver _findDriverByDeviceName(String deviceName) {
        for (UsbSerialDriver driver: _drivers) {
            if (driver.getDevice().getDeviceName().equals(deviceName)) {
                return driver;
            }
        }
        return null;
    }

    private final static BroadcastReceiver _usbReceiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                Log.i(TAG, "BroadcastReceiver USB action " + action);

                if (ACTION_USB_PERMISSION.equals(action)) {
                    synchronized (_instance) {
                        UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                        if (device != null) {
                            UsbSerialDriver driver = _findDriverByDeviceId(device.getDeviceId());

                            if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                                qgcLogDebug("Permission granted to " + device.getDeviceName());
                                driver.setPermissionStatus(UsbSerialDriver.permissionStatusSuccess);
                            } else {
                                qgcLogDebug("Permission denied for " + device.getDeviceName());
                                driver.setPermissionStatus(UsbSerialDriver.permissionStatusDenied);
                            }
                        }
                    }
                } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device != null) {
                        if (_userDataHashByDeviceId.containsKey(device.getDeviceId())) {
                            nativeDeviceHasDisconnected(_userDataHashByDeviceId.get(device.getDeviceId()));
                        }
                    }
                }

                try {
                    nativeUpdateAvailableJoysticks();
                } catch(Exception e) {
                    Log.e(TAG, "Exception nativeUpdateAvailableJoysticks()");
                }
            }
        };

    // Native C++ functions which connect back to QSerialPort code
    private static native void nativeDeviceHasDisconnected(long userData);
    private static native void nativeDeviceException(long userData, String messageA);
    private static native void nativeDeviceNewData(long userData, byte[] dataA);
    private static native void nativeUpdateAvailableJoysticks();

    // Native C++ functions called to log output
    public static native void qgcLogDebug(String message);
    public static native void qgcLogWarning(String message);

    public native void nativeInit();

    // QGCActivity singleton
    public QGCActivity()
    {
        _instance =                 this;
        _drivers =                  new ArrayList<UsbSerialDriver>();
        _userDataHashByDeviceId =   new HashMap<Integer, Long>();
        m_ioManager =               new HashMap<Integer, UsbIoManager>();
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        nativeInit();
        PowerManager pm = (PowerManager)_instance.getSystemService(Context.POWER_SERVICE);
        _wakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "QGroundControl");
        if(_wakeLock != null) {
            _wakeLock.acquire();
        } else {
            Log.i(TAG, "SCREEN_BRIGHT_WAKE_LOCK not acquired!!!");
        }
        _instance.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        _usbManager = (UsbManager)_instance.getSystemService(Context.USB_SERVICE);

        // Register for USB Detach and USB Permission intent
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        filter.addAction(ACTION_USB_PERMISSION);
        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        _instance.registerReceiver(_instance._usbReceiver, filter);

        // Create intent for usb permission request
        _usbPermissionIntent = PendingIntent.getBroadcast(_instance, 0, new Intent(ACTION_USB_PERMISSION), 0);

        try {
            taiSync = new TaiSync();

            IntentFilter accessoryFilter = new IntentFilter(ACTION_USB_PERMISSION);
            filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
            registerReceiver(mOpenAccessoryReceiver, accessoryFilter);

            probeAccessoriesTimer = new Timer();
            probeAccessoriesTimer.schedule(new TimerTask() {
                @Override
                public void run()
                {
                    probeAccessories();
                }
            }, 0, 3000);
        } catch(Exception e) {
           Log.e(TAG, "Exception: " + e);
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        // Plug in of USB ACCESSORY triggers only onResume event.
        // Then we scan if there is actually anything new
        probeAccessories();
    }

    @Override
    protected void onDestroy()
    {
        if (probeAccessoriesTimer != null) {
            probeAccessoriesTimer.cancel();
        }
        unregisterReceiver(mOpenAccessoryReceiver);
        try {
            if(_wakeLock != null) {
                _wakeLock.release();
            }
        } catch(Exception e) {
           Log.e(TAG, "Exception onDestroy()");
        }
        super.onDestroy();
    }

    public void onInit(int status) {
    }

    /// Incrementally updates the list of drivers connected to the device
    private static void updateCurrentDrivers()
    {
        List<UsbSerialDriver> currentDrivers = UsbSerialProber.findAllDevices(_usbManager);

        // Remove stale drivers
        for (int i=_drivers.size()-1; i>=0; i--) {
            boolean found = false;
            for (UsbSerialDriver currentDriver: currentDrivers) {
                if (_drivers.get(i).getDevice().getDeviceId() == currentDriver.getDevice().getDeviceId()) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                qgcLogDebug("Remove stale driver " + _drivers.get(i).getDevice().getDeviceName());
                _drivers.remove(i);
            }
        }

        // Add new drivers
        for (int i=0; i<currentDrivers.size(); i++) {
            boolean found = false;
            for (int j=0; j<_drivers.size(); j++) {
                if (currentDrivers.get(i).getDevice().getDeviceId() == _drivers.get(j).getDevice().getDeviceId()) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                UsbSerialDriver newDriver =     currentDrivers.get(i);
                UsbDevice       device =        newDriver.getDevice();
                String          deviceName =    device.getDeviceName();

                _drivers.add(newDriver);
                qgcLogDebug("Adding new driver " + deviceName);

                // Request permission if needed
                if (_usbManager.hasPermission(device)) {
                    qgcLogDebug("Already have permission to use device " + deviceName);
                    newDriver.setPermissionStatus(UsbSerialDriver.permissionStatusSuccess);
                } else {
                    qgcLogDebug("Requesting permission to use device " + deviceName);
                    newDriver.setPermissionStatus(UsbSerialDriver.permissionStatusRequested);
                    _usbManager.requestPermission(device, _usbPermissionIntent);
                }
            }
        }
    }

    /// Returns array of device info for each unopened device.
    /// @return Device info format DeviceName:Company:ProductId:VendorId
    public static String[] availableDevicesInfo()
    {
        updateCurrentDrivers();

        if (_drivers.size() <= 0) {
            return null;
        }

        List<String> deviceInfoList = new ArrayList<String>();

        for (int i=0; i<_drivers.size(); i++) {
            String          deviceInfo;
            UsbSerialDriver driver = _drivers.get(i);

            if (driver.permissionStatus() != UsbSerialDriver.permissionStatusSuccess) {
                continue;
            }

            UsbDevice device = driver.getDevice();

            deviceInfo = device.getDeviceName() + ":";

            if (driver instanceof FtdiSerialDriver) {
                deviceInfo = deviceInfo + "FTDI:";
            } else if (driver instanceof CdcAcmSerialDriver) {
                deviceInfo = deviceInfo + "Cdc Acm:";
            } else if (driver instanceof Cp2102SerialDriver) {
                deviceInfo = deviceInfo + "Cp2102:";
            } else if (driver instanceof ProlificSerialDriver) {
                deviceInfo = deviceInfo + "Prolific:";
            } else {
                deviceInfo = deviceInfo + "Unknown:";
            }

            deviceInfo = deviceInfo + Integer.toString(device.getProductId()) + ":";
            deviceInfo = deviceInfo + Integer.toString(device.getVendorId()) + ":";

            deviceInfoList.add(deviceInfo);
        }

        String[] rgDeviceInfo = new String[deviceInfoList.size()];
        for (int i=0; i<deviceInfoList.size(); i++) {
            rgDeviceInfo[i] = deviceInfoList.get(i);
        }

        return rgDeviceInfo;
    }

    /// Open the specified device
    ///     @param userData Data to associate with device and pass back through to native calls.
    /// @return Device id
    public static int open(Context parentContext, String deviceName, long userData)
    {
        int deviceId = BAD_DEVICE_ID;

        m_context = parentContext;

        UsbSerialDriver driver = _findDriverByDeviceName(deviceName);
        if (driver == null) {
            qgcLogWarning("Attempt to open unknown device " + deviceName);
            return BAD_DEVICE_ID;
        }

        if (driver.permissionStatus() != UsbSerialDriver.permissionStatusSuccess) {
            qgcLogWarning("Attempt to open device with incorrect permission status " + deviceName + " " + driver.permissionStatus());
            return BAD_DEVICE_ID;
        }

        UsbDevice device = driver.getDevice();
        deviceId = device.getDeviceId();

        try {
            driver.setConnection(_usbManager.openDevice(device));
            driver.open();
            driver.setPermissionStatus(UsbSerialDriver.permissionStatusOpen);

            _userDataHashByDeviceId.put(deviceId, userData);

            UsbIoManager ioManager = new UsbIoManager(driver, m_Listener, userData);
            m_ioManager.put(deviceId, ioManager);
            m_Executor.submit(ioManager);

            qgcLogDebug("Port open successful");
        } catch(IOException exA) {
            driver.setPermissionStatus(UsbSerialDriver.permissionStatusRequestRequired);
            _userDataHashByDeviceId.remove(deviceId);

            if(m_ioManager.get(deviceId) != null) {
                m_ioManager.get(deviceId).stop();
                m_ioManager.remove(deviceId);
            }
            qgcLogWarning("Port open exception: " + exA.getMessage());
            return BAD_DEVICE_ID;
        }

        return deviceId;
    }

    public static void startIoManager(int idA)
    {
        if (m_ioManager.get(idA) != null)
            return;

        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return;

        UsbIoManager managerL = new UsbIoManager(driverL, m_Listener, _userDataHashByDeviceId.get(idA));
        m_ioManager.put(idA, managerL);
        m_Executor.submit(managerL);
    }

    public static void stopIoManager(int idA)
    {
        if(m_ioManager.get(idA) == null)
            return;

        m_ioManager.get(idA).stop();
        m_ioManager.remove(idA);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Sets the parameters on an open port.
    //
    //  Args:   idA - ID number from the open command
    //          baudRateA - Decimal value of the baud rate.  I.E. 9600, 57600, 115200, etc.
    //          dataBitsA - number of data bits.  Valid numbers are 5, 6, 7, 8
    //          stopBitsA - number of stop bits.  Valid numbers are 1, 2
    //          parityA - No Parity=0, Odd Parity=1, Even Parity=2
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean setParameters(int idA, int baudRateA, int dataBitsA, int stopBitsA, int parityA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return false;

        try
        {
            driverL.setParameters(baudRateA, dataBitsA, stopBitsA, parityA);
            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Close the device.
    //
    //  Args:  idA - ID number from the open command
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean close(int idA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return false;

        try
        {
            stopIoManager(idA);
            _userDataHashByDeviceId.remove(idA);
            driverL.setPermissionStatus(UsbSerialDriver.permissionStatusRequestRequired);
            driverL.close();

            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Write data to the device.
    //
    //  Args:   idA - ID number from the open command
    //          sourceA - byte array of data to write
    //          timeoutMsecA - amount of time in milliseconds to wait for the write to occur
    //
    //  Returns:  number of bytes written
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    public static int write(int idA, byte[] sourceA, int timeoutMSecA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return 0;

        try
        {
            return driverL.write(sourceA, timeoutMSecA);
        }
        catch(IOException eA)
        {
            return 0;
        }
        /*
        UsbIoManager managerL = m_ioManager.get(idA);

        if(managerL != null)
        {
            managerL.writeAsync(sourceA);
            return sourceA.length;
        }
        else
            return 0;
        */
    }

    public static boolean isDeviceNameValid(String nameA)
    {
        for (UsbSerialDriver driver: _drivers) {
            if (driver.getDevice().getDeviceName() == nameA)
                return true;
        }

        return false;
    }

    public static boolean isDeviceNameOpen(String nameA)
    {
        for (UsbSerialDriver driverL: _drivers) {
            if (nameA.equals(driverL.getDevice().getDeviceName()) && driverL.permissionStatus() == UsbSerialDriver.permissionStatusOpen) {
                return true;
            }
        }

        return false;
    }



    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Set the Data Terminal Ready flag on the device
    //
    //  Args:   idA - ID number from the open command
    //          onA - on=T, off=F
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean setDataTerminalReady(int idA, boolean onA)
    {
        try
        {
            UsbSerialDriver driverL = _findDriverByDeviceId(idA);

            if (driverL == null)
                return false;

            driverL.setDTR(onA);
            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Set the Request to Send flag
    //
    //  Args:   idA - ID number from the open command
    //          onA - on=T, off=F
    //
    //  Returns:  T/F Success/Failure
    //
    ////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean setRequestToSend(int idA, boolean onA)
    {
        try
        {
            UsbSerialDriver driverL = _findDriverByDeviceId(idA);

            if (driverL == null)
                return false;

            driverL.setRTS(onA);
            return true;
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Purge the hardware buffers based on the input and output flags
    //
    //  Args:   idA - ID number from the open command
    //          inputA - input buffer purge.  purge=T
    //          outputA - output buffer purge.  purge=T
    //
    //  Returns:  T/F Success/Failure
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean purgeBuffers(int idA, boolean inputA, boolean outputA)
    {
        try
        {
            UsbSerialDriver driverL = _findDriverByDeviceId(idA);

            if (driverL == null)
                return false;

            return driverL.purgeHwBuffers(inputA, outputA);
        }
        catch(IOException eA)
        {
            return false;
        }
    }



    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Get the native device handle (file descriptor)
    //
    //  Args:   idA - ID number from the open command
    //
    //  Returns:  device handle
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    public static int getDeviceHandle(int idA)
    {
        UsbSerialDriver driverL = _findDriverByDeviceId(idA);

        if (driverL == null)
            return -1;

        UsbDeviceConnection connectL = driverL.getDeviceConnection();
        if (connectL == null)
            return -1;
        else
            return connectL.getFileDescriptor();
    }

    UsbAccessory openUsbAccessory = null;
    Object openAccessoryLock = new Object();

    private void openAccessory(UsbAccessory usbAccessory)
    {
        Log.i(TAG, "openAccessory: " + usbAccessory.getSerial());
        try {
            synchronized(openAccessoryLock) {
                if ((openUsbAccessory != null && !taiSync.isRunning()) || openUsbAccessory == null) {
                    openUsbAccessory = usbAccessory;
                    taiSync.open(_usbManager.openAccessory(usbAccessory));
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "openAccessory exception: " + e);
            taiSync.close();
            closeAccessory(openUsbAccessory);
        }
    }

    private void closeAccessory(UsbAccessory usbAccessory)
    {
        Log.i(TAG, "closeAccessory");

        synchronized(openAccessoryLock) {
            if (openUsbAccessory != null && usbAccessory == openUsbAccessory && taiSync.isRunning()) {
                taiSync.close();
                openUsbAccessory = null;
            }
        }
    }

    Object probeAccessoriesLock = new Object();

    private void probeAccessories()
    {
        final PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
        new Thread(new Runnable() {
            public void run() {
                synchronized(openAccessoryLock) {
//                    Log.i(TAG, "probeAccessories");
                    UsbAccessory[] accessories = _usbManager.getAccessoryList();
                    if (accessories != null) {
                       for (UsbAccessory usbAccessory : accessories) {
                           if (usbAccessory == null) {
                               continue;
                           }
                           if (_usbManager.hasPermission(usbAccessory)) {
                               openAccessory(usbAccessory);
                           }
                       }
                    }
                }
            }
        }).start();
    }
}
      - name: package org.mavlink.qgroundcontrol;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Socket;
import java.net.InetAddress;

import android.os.ParcelFileDescriptor;
import android.util.Log;

public class TaiSync
{
    private static final int HEADER_SIZE = 0x1C;

    private static final byte PROTOCOL_REQUEST_CONNECTION = 0x00;
    private static final byte PROTOCOL_VERSION = 0x01;
    private static final byte PROTOCOL_CHANNEL = 0x02;
    private static final byte PROTOCOL_DATA = 0x03;

    private static final int VIDEO_PORT = 5600;
    private static final int TAISYNC_VIDEO_PORT = 8000;
    private static final int TAISYNC_SETTINGS_PORT = 8200;
    private static final int TAISYNC_TELEMETRY_PORT = 8400;

    private Object runLock;
    private boolean running = false;
    private DatagramSocket udpSocket = null;
    private Socket tcpSettingsSocket = null;
    private InputStream settingsInStream = null;
    private OutputStream settingsOutStream = null;
    private Socket tcpTelemetrySocket = null;
    private InputStream telemetryInStream = null;
    private OutputStream telemetryOutStream = null;
    private ParcelFileDescriptor mParcelFileDescriptor;
    private FileInputStream mFileInputStream;
    private FileOutputStream mFileOutputStream;
    private ExecutorService mThreadPool;
    private byte[] mBytes = new byte[32 * 1024];
    private byte vMaj = 0;

    public TaiSync()
    {
        runLock = new Object();
        mThreadPool = Executors.newFixedThreadPool(3);
    }

    public boolean isRunning()
    {
        synchronized (runLock) {
            return running;
        }
    }

    public void open(ParcelFileDescriptor parcelFileDescriptor) throws IOException
    {
//        Log.i("QGC_TaiSync", "Open");

        synchronized (runLock) {
            if (running) {
                return;
            }
            running = true;
        }

        mParcelFileDescriptor = parcelFileDescriptor;
        if (mParcelFileDescriptor == null) {
            throw new IOException("parcelFileDescriptor is null");
        }

        FileDescriptor fileDescriptor = mParcelFileDescriptor.getFileDescriptor();
        mFileInputStream = new FileInputStream(fileDescriptor);
        mFileOutputStream = new FileOutputStream(fileDescriptor);

        udpSocket = new DatagramSocket();
        final InetAddress address = InetAddress.getByName("localhost");
        tcpTelemetrySocket = new Socket(address, TAISYNC_TELEMETRY_PORT);
        telemetryInStream = tcpTelemetrySocket.getInputStream();
        telemetryOutStream = tcpTelemetrySocket.getOutputStream();
        tcpSettingsSocket = new Socket(address, TAISYNC_SETTINGS_PORT);
        settingsInStream = tcpSettingsSocket.getInputStream();
        settingsOutStream = tcpSettingsSocket.getOutputStream();

        // Request connection packet
        sendTaiSyncMessage(PROTOCOL_REQUEST_CONNECTION, 0, null, 0);

        // Read multiplexed data stream coming from TaiSync accessory
        mThreadPool.execute(new Runnable() {
            @Override
            public void run()
            {
                int bytesRead = 0;

                try {
                    while (bytesRead >= 0) {
                        synchronized (runLock) {
                            if (!running) {
                                break;
                            }
                        }

                        bytesRead = mFileInputStream.read(mBytes);

                        if (bytesRead > 0)
                        {
                            if (mBytes[3] == PROTOCOL_VERSION)
                            {
                                vMaj = mBytes[19];
                                // Log.i("QGC_TaiSync", "Got protocol version message vMaj = " + mBytes[19]);
                                sendTaiSyncMessage(PROTOCOL_VERSION, 0, null, 0);
                            }
                            else if (mBytes[3] == PROTOCOL_CHANNEL) {
                                int dPort = ((mBytes[4] & 0xff)<< 24) | ((mBytes[5]&0xff) << 16) | ((mBytes[6]&0xff) << 8) | (mBytes[7] &0xff);
                                int dLength = ((mBytes[8] & 0xff)<< 24) | ((mBytes[9]&0xff) << 16) | ((mBytes[10]&0xff) << 8) | (mBytes[11] &0xff);
                                // Log.i("QGC_TaiSync", "Read 2 port = " + dPort + " length = " + dLength);
                                sendTaiSyncMessage(PROTOCOL_CHANNEL, dPort, null, 0);
                            }
                            else if (mBytes[3] == PROTOCOL_DATA) {
                                int dPort = ((mBytes[4] & 0xff)<< 24) | ((mBytes[5]&0xff) << 16) | ((mBytes[6]&0xff) << 8) | (mBytes[7] &0xff);
                                int dLength = ((mBytes[8] & 0xff)<< 24) | ((mBytes[9]&0xff) << 16) | ((mBytes[10]&0xff) << 8) | (mBytes[11] &0xff);

                                int payloadOffset = HEADER_SIZE;
                                int payloadLength = bytesRead - payloadOffset;

                                byte[] sBytes = new byte[payloadLength];
                                System.arraycopy(mBytes, payloadOffset, sBytes, 0, payloadLength);

                                if (dPort == TAISYNC_VIDEO_PORT) {
                                    DatagramPacket packet = new DatagramPacket(sBytes, sBytes.length, address, VIDEO_PORT);
                                    udpSocket.send(packet);
                                } else if (dPort == TAISYNC_SETTINGS_PORT) {
                                    settingsOutStream.write(sBytes);
                                } else if (dPort == TAISYNC_TELEMETRY_PORT) {
                                    telemetryOutStream.write(sBytes);
                                }
                            }
                        }
                    }
                } catch (IOException e) {
                    Log.e("QGC_TaiSync", "Exception: " + e);
                    e.printStackTrace();
                } finally {
                    close();
                }
            }
        });

        // Read command & control packets to be sent to telemetry port
        mThreadPool.execute(new Runnable() {
            @Override
            public void run()
            {
                byte[] inbuf = new byte[256];

                try {
                    while (true) {
                        synchronized (runLock) {
                            if (!running) {
                                break;
                            }
                        }

                        int bytesRead = telemetryInStream.read(inbuf, 0, inbuf.length);
                        if (bytesRead > 0) {
                            sendTaiSyncMessage(PROTOCOL_DATA, TAISYNC_TELEMETRY_PORT, inbuf, bytesRead);
                        }
                    }
                } catch (IOException e) {
                    Log.e("QGC_TaiSync", "Exception: " + e);
                    e.printStackTrace();
                } finally {
                    close();
                }
            }
        });

        // Read incoming requests for settings socket
        mThreadPool.execute(new Runnable() {
            @Override
            public void run()
            {
                byte[] inbuf = new byte[1024];

                try {
                    while (true) {
                        synchronized (runLock) {
                            if (!running) {
                                break;
                            }
                        }

                        int bytesRead = settingsInStream.read(inbuf, 0, inbuf.length);
                        if (bytesRead > 0) {
                            sendTaiSyncMessage(PROTOCOL_DATA, TAISYNC_SETTINGS_PORT, inbuf, bytesRead);
                        }
                    }
                } catch (IOException e) {
                    Log.e("QGC_TaiSync", "Exception: " + e);
                    e.printStackTrace();
                } finally {
                    close();
                }
            }
        });
   }

    private void sendTaiSyncMessage(byte protocol, int dataPort, byte[] data, int dataLen) throws IOException
    {
        byte portMSB = (byte)((dataPort >> 8) & 0xFF);
        byte portLSB = (byte)(dataPort & 0xFF);

        byte[] lA = new byte[4];
        int len = HEADER_SIZE + dataLen;
        // Log.i("QGC_TaiSync", "Sending to " + dataPort + " length = " + len);
        byte[] buffer = new byte[len];

        for (int i = 3; i >= 0; i--) {
            lA[i] = (byte)(len & 0xFF);
            len >>= 8;
        }

        byte[] header = { 0x00, 0x00, 0x00, protocol,   // uint32 - protocol
                          0x00, 0x00, portMSB, portLSB, // uint32 - dport
                          lA[0], lA[1], lA[2], lA[3],   // uint32 - length
                          0x00, 0x00, 0x00, 0x00,       // uint32 - magic
                          0x00, 0x00, 0x00, vMaj,       // uint32 - version major
                          0x00, 0x00, 0x00, 0x00,       // uint32 - version minor
                          0x00, 0x00, 0x00, 0x00 };     // uint32 - padding

        System.arraycopy(header, 0, buffer, 0, header.length);
        if (data != null && dataLen > 0) {
            System.arraycopy(data, 0, buffer, header.length, dataLen);
        }

        synchronized (runLock) {
            mFileOutputStream.write(buffer);
        }
    }

    public void close()
    {
//        Log.i("QGC_TaiSync", "Close");
        synchronized (runLock) {
            running = false;
        }
        try {
            if (udpSocket != null) {
                udpSocket.close();
            }
        } catch (Exception e) {
        }
        try {
            if (tcpTelemetrySocket != null) {
                tcpTelemetrySocket.close();
            }
        } catch (Exception e) {
        }
        try {
            if (tcpSettingsSocket != null) {
                tcpSettingsSocket.close();
            }
        } catch (Exception e) {
        }
        try {
            if (mParcelFileDescriptor != null) {
                mParcelFileDescriptor.close();
            }
        } catch (Exception e) {
        }
        udpSocket = null;
        tcpSettingsSocket = null;
        tcpTelemetrySocket = null;
        settingsInStream = null;
        settingsOutStream = null;
        mParcelFileDescriptor = null;
    }
}
        uses: package org.mavlink.qgroundcontrol;

/* Copyright 2011 Google Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
* USA.
*
* Project home page: http://code.google.com/p/usb-serial-for-android/
*/


import com.hoho.android.usbserial.driver.*;

import java.io.IOException;
import java.nio.ByteBuffer;
import android.util.Log;

/**
* Utility class which services a {@link UsbSerialDriver} in its {@link #run()}
* method.
*
* Original author mike wakerly (opensource@hoho.com)
* Modified by Mike Goza
*/
public class UsbIoManager implements Runnable {
    private static final int READ_WAIT_MILLIS = 100;
    private static final int BUFSIZ = 4096;
    private static final String TAG = "QGC_UsbIoManager";

    private final UsbSerialDriver mDriver;
    private long mUserData;
    private final ByteBuffer mReadBuffer = ByteBuffer.allocate(BUFSIZ);
    private final ByteBuffer mWriteBuffer = ByteBuffer.allocate(BUFSIZ);

    private enum State
    {
        STOPPED,
        RUNNING,
        STOPPING
    }

    // Synchronized by 'this'
    private State mState = State.STOPPED;

    // Synchronized by 'this'
    private Listener mListener;

    public interface Listener
    {
        /**
         * Called when new incoming data is available.
         */
        public void onNewData(byte[] data, long userData);

        /**
         * Called when {@link SerialInputOutputManager#run()} aborts due to an
         * error.
         */
        public void onRunError(Exception e, long userData);
    }



    /**
     * Creates a new instance with no listener.
     */
    public UsbIoManager(UsbSerialDriver driver)
    {
        this(driver, null, 0);
        Log.i(TAG, "Instance created");
    }



    /**
     * Creates a new instance with the provided listener.
     */
    public UsbIoManager(UsbSerialDriver driver, Listener listener, long userData)
    {
        mDriver = driver;
        mListener = listener;
        mUserData = userData;
    }



    public synchronized void setListener(Listener listener)
    {
        mListener = listener;
    }



    public synchronized Listener getListener()
    {
        return mListener;
    }



    public void writeAsync(byte[] data)
    {
        synchronized (mWriteBuffer)
        {
            mWriteBuffer.put(data);
        }
    }



    public synchronized void stop()
    {
        if (getState() == State.RUNNING)
        {
            mState = State.STOPPING;
            mUserData = 0;
        }
    }



    private synchronized State getState()
    {
        return mState;
    }



    /**
     * Continuously services the read and write buffers until {@link #stop()} is
     * called, or until a driver exception is raised.
     */
    @Override
    public void run()
    {
        synchronized (this)
        {
            if (mState != State.STOPPED)
                throw new IllegalStateException("Already running.");

            mState = State.RUNNING;
        }

        try
        {
            while (true)
            {
                if (mState != State.RUNNING)
                    break;

                step();
            }
        }
        catch (Exception e)
        {
            final Listener listener = getListener();
            if (listener != null)
                listener.onRunError(e, mUserData);
        }
        finally
        {
            synchronized (this)
            {
                mState = State.STOPPED;
            }
        }
    }



    private void step() throws IOException
    {
        // Handle incoming data.
        int len = mDriver.read(mReadBuffer.array(), READ_WAIT_MILLIS);

        if (len > 0)
        {
            final Listener listener = getListener();
            if (listener != null)
            {
                final byte[] data = new byte[len];
                mReadBuffer.get(data, 0, len);
                listener.onNewData(data, mUserData);
            }
            mReadBuffer.clear();
        }
/*
        // Handle outgoing data.
        byte[] outBuff = null;
        synchronized (mWriteBuffer)
        {
            if (mWriteBuffer.position() > 0)
            {
                len = mWriteBuffer.position();
                outBuff = new byte[len];
                mWriteBuffer.rewind();
                mWriteBuffer.get(outBuff, 0, len);
                mWriteBuffer.clear();
            }
        }
        if (outBuff != null)
            mDriver.write(outBuff, READ_WAIT_MILLIS);
*/
    }
}
      - name: <?xml version="1.0"?>
<manifest package="org.mavlink.qgroundcontrol" xmlns:android="http://schemas.android.com/apk/res/android" android:versionName="3.0.0-243-gd759437" android:versionCode="300243" android:installLocation="auto">
    <application android:hardwareAccelerated="true" android:name="org.qtproject.qt5.android.bindings.QtApplication" android:label="-- %%INSERT_APP_NAME%% --" android:icon="@drawable/icon">
        <activity android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|locale|fontScale|keyboard|keyboardHidden|navigation" android:name="org.mavlink.qgroundcontrol.QGCActivity" android:label="-- %%INSERT_APP_NAME%% --" android:screenOrientation="sensorLandscape" android:launchMode="singleTask" android:keepScreenOn="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
                <action android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"/>
                <action android:name="android.hardware.usb.action.USB_DEVICE_DETACHED"/>
                <action android:name="android.bluetooth.device.action.ACL_CONNECTED" />
                <action android:name="android.bluetooth.device.action.ACL_DISCONNECTED" />
                <action android:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED"/>
            </intent-filter>

            <meta-data android:resource="@xml/device_filter" android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"/>
            <meta-data android:resource="@xml/device_filter" android:name="android.hardware.usb.action.USB_DEVICE_DETACHED"/>
            <meta-data android:resource="@xml/device_filter" android:name="android.hardware.usb.action.USB_ACCESSORY_ATTACHED"/>
            <!-- Rest of Standard Manifest -->
            <meta-data android:name="android.app.lib_name" android:value="-- %%INSERT_APP_LIB_NAME%% --"/>
            <meta-data android:name="android.app.qt_sources_resource_id" android:resource="@array/qt_sources"/>
            <meta-data android:name="android.app.repository" android:value="default"/>
            <meta-data android:name="android.app.qt_libs_resource_id" android:resource="@array/qt_libs"/>
            <meta-data android:name="android.app.bundled_libs_resource_id" android:resource="@array/bundled_libs"/>
            <!-- Deploy Qt libs as part of package -->
            <meta-data android:name="android.app.bundle_local_qt_libs" android:value="-- %%BUNDLE_LOCAL_QT_LIBS%% --"/>
            <meta-data android:name="android.app.bundled_in_lib_resource_id" android:resource="@array/bundled_in_lib"/>
            <meta-data android:name="android.app.bundled_in_assets_resource_id" android:resource="@array/bundled_in_assets"/>
            <!-- Run with local libs -->
            <meta-data android:name="android.app.use_local_qt_libs" android:value="-- %%USE_LOCAL_QT_LIBS%% --"/>
            <meta-data android:name="android.app.libs_prefix" android:value="/data/local/tmp/qt/"/>
            <meta-data android:name="android.app.load_local_libs" android:value="-- %%INSERT_LOCAL_LIBS%% --"/>
            <meta-data android:name="android.app.load_local_jars" android:value="-- %%INSERT_LOCAL_JARS%% --"/>
            <meta-data android:name="android.app.static_init_classes" android:value="-- %%INSERT_INIT_CLASSES%% --"/>
            <!--  Messages maps -->
            <meta-data android:value="@string/ministro_not_found_msg" android:name="android.app.ministro_not_found_msg"/>
            <meta-data android:value="@string/ministro_needed_msg" android:name="android.app.ministro_needed_msg"/>
            <meta-data android:value="@string/fatal_error_msg" android:name="android.app.fatal_error_msg"/>
            <!--  Messages maps -->

            <!-- Splash screen -->
            <!--
            <meta-data android:name="android.app.splash_screen_drawable" android:resource="@drawable/logo"/>
            -->
            <!-- Splash screen -->

            <!-- Background running -->
            <!-- Warning: changing this value to true may cause unexpected crashes if the
                          application still try to draw after
                          "applicationStateChanged(Qt::ApplicationSuspended)"
                          signal is sent! -->
            <meta-data android:name="android.app.background_running" android:value="false"/>
            <!-- Background running -->
        </activity>
    </application>
    <uses-sdk android:minSdkVersion="16" android:targetSdkVersion="28"/>

    <!-- Needed to keep working while 'asleep' -->
    <uses-permission android:name="android.permission.WAKE_LOCK"/>

    <!-- The following comment will be replaced upon deployment with default permissions based on the dependencies of the application.
         Remove the comment if you do not require these default permissions. -->
    <!-- %%INSERT_PERMISSIONS -->

    <!-- Support devices without USB host mode since there are other connection types -->
    <uses-feature android:name="android.hardware.usb.host" android:required="false"/>

    <!-- Support devices without Bluetooth since there are other connection types -->
    <uses-feature android:name="android.hardware.bluetooth" android:required="false"/>

    <!-- Support devices that don't have location services -->
    <uses-feature android:name="android.hardware.location.gps" android:required="false" />
    <uses-feature android:name="android.hardware.location.network" android:required="false"/>
    <uses-feature android:name="android.hardware.location" android:required="false"/>
    <uses-feature android:name="android.hardware.usb.accessory"/>

        <uses-permission android:name="android.permission.INTERNET" />
        <uses-permission android:name="android.permission.WRITE_INTERNAL_STORAGE" />
        <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

    <!-- The following comment will be replaced upon deployment with default features based on the dependencies of the application.
         Remove the comment if you do not require these default features. -->
    <!-- %%INSERT_FEATURES -->

</manifest>
        uses: <?xml version="1.0"?>
<manifest android:versionName="@QT_ANDROID_APP_VERSION@" package="@QT_ANDROID_APP_PACKAGE_NAME@" android:installLocation="auto" xmlns:android="http://schemas.android.com/apk/res/android" android:versionCode="@QT_ANDROID_APP_VERSION_CODE@">
    <application android:label="@QT_ANDROID_APP_NAME@" android:name="org.qtproject.qt5.android.bindings.QtApplication">
                <activity android:label="@QT_ANDROID_APP_NAME@" android:name="org.qtproject.qt5.android.bindings.QtActivity" android:screenOrientation="unspecified" android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|locale|fontScale|keyboard|keyboardHidden|navigation">
                        <intent-filter>
                                <action android:name="android.intent.action.MAIN"/>
                                <category android:name="android.intent.category.LAUNCHER"/>
                        </intent-filter>
                        <meta-data android:name="android.app.lib_name" android:value="-- %%INSERT_APP_LIB_NAME%% --"/>
                        <meta-data android:name="android.app.qt_sources_resource_id" android:resource="@array/qt_sources"/>
                        <meta-data android:name="android.app.repository" android:value="default"/>
                        <meta-data android:name="android.app.qt_libs_resource_id" android:resource="@array/qt_libs"/>
                        <meta-data android:name="android.app.bundled_libs_resource_id" android:resource="@array/bundled_libs"/>
                        <!-- Deploy Qt libs as part of package -->
                        <meta-data android:name="android.app.bundle_local_qt_libs" android:value="-- %%BUNDLE_LOCAL_QT_LIBS%% --"/>
                        <meta-data android:name="android.app.bundled_in_lib_resource_id" android:resource="@array/bundled_in_lib"/>
                        <meta-data android:name="android.app.bundled_in_assets_resource_id" android:resource="@array/bundled_in_assets"/>
                        <!-- Run with local libs -->
                        <meta-data android:name="android.app.use_local_qt_libs" android:value="-- %%USE_LOCAL_QT_LIBS%% --"/>
                        <meta-data android:name="android.app.libs_prefix" android:value="/data/local/tmp/qt/"/>
                        <meta-data android:name="android.app.load_local_libs" android:value="-- %%INSERT_LOCAL_LIBS%% --"/>
                        <meta-data android:name="android.app.load_local_jars" android:value="-- %%INSERT_LOCAL_JARS%% --"/>
                        <meta-data android:name="android.app.static_init_classes" android:value="-- %%INSERT_INIT_CLASSES%% --"/>
                        <!--  Messages maps -->
                        <!--<meta-data android:name="android.app.ministro_not_found_msg" android:value="@string/ministro_not_found_msg"/>
                        <meta-data android:name="android.app.ministro_needed_msg" android:value="@string/ministro_needed_msg"/>
                        <meta-data android:name="android.app.fatal_error_msg" android:value="@string/fatal_error_msg"/>-->
                </activity>
        </application>
        <supports-screens android:anyDensity="true" android:normalScreens="true" android:smallScreens="true" android:largeScreens="true"/>
        <uses-sdk android:minSdkVersion="18" android:targetSdkVersion="19"/>
        <uses-permission android:name="android.permission.INTERNET" />
</manifest>
        with: buildscript {

    repositories {
        maven  {
            url "https://repo1.maven.org/maven2"
        }
    }

    dependencies {
        classpath 'com.android.tools.build:gradle:1.1.0'
    }
}

allprojects {
    repositories {
        jcenter()
    }
}

apply plugin: 'com.android.application'

dependencies {
    compile fileTree(dir: 'libs', include: ['*.jar'])
}

android {
    /*******************************************************
     * The following variables:
     * - androidBuildToolsVersion,
     * - androidCompileSdkVersion
     * - qt5AndroidDir - holds the path to qt android files
     *                   needed to build any Qt application
     *                   on Android.
     *
     * are defined in gradle.properties file. This file is
     * updated by QtCreator and androiddeployqt tools.
     * Changing them manually might break the compilation!
     *******************************************************/

    compileSdkVersion androidCompileSdkVersion.toInteger()

    buildToolsVersion androidBuildToolsVersion

    sourceSets {
        main {
            manifest.srcFile 'AndroidManifest.xml'
            java.srcDirs = [qt5AndroidDir + '/src', 'src', 'java']
            aidl.srcDirs = [qt5AndroidDir + '/src', 'src', 'aidl']
            res.srcDirs = [qt5AndroidDir + '/res', 'res']
            resources.srcDirs = ['src']
            renderscript.srcDirs = ['src']
            assets.srcDirs = ['assets']
            jniLibs.srcDirs = ['libs']
       }
    }

    aaptOptions {
        cruncherEnabled = false
    }

    lintOptions {
        abortOnError false
    }
}
          source: #!/usr/bin/env bash

##############################################################################
##
##  Gradle start up script for UN*X
##
##############################################################################

# Add default JVM options here. You can also use JAVA_OPTS and GRADLE_OPTS to pass JVM options to this script.
DEFAULT_JVM_OPTS=""

APP_NAME="Gradle"
APP_BASE_NAME=`basename "$0"`

# Use the maximum available, or set MAX_FD != -1 to use that value.
MAX_FD="maximum"

warn ( ) {
    echo "$*"
}

die ( ) {
    echo
    echo "$*"
    echo
    exit 1
}

# OS specific support (must be 'true' or 'false').
cygwin=false
msys=false
darwin=false
case "`uname`" in
  CYGWIN* )
    cygwin=true
    ;;
  Darwin* )
    darwin=true
    ;;
  MINGW* )
    msys=true
    ;;
esac

# For Cygwin, ensure paths are in UNIX format before anything is touched.
if $cygwin ; then
    [ -n "$JAVA_HOME" ] && JAVA_HOME=`cygpath --unix "$JAVA_HOME"`
fi

# Attempt to set APP_HOME
# Resolve links: $0 may be a link
PRG="$0"
# Need this for relative symlinks.
while [ -h "$PRG" ] ; do
    ls=`ls -ld "$PRG"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '/.*' > /dev/null; then
        PRG="$link"
    else
        PRG=`dirname "$PRG"`"/$link"
    fi
done
SAVED="`pwd`"
cd "`dirname \"$PRG\"`/" >&-
APP_HOME="`pwd -P`"
cd "$SAVED" >&-

CLASSPATH=$APP_HOME/gradle/wrapper/gradle-wrapper.jar

# Determine the Java command to use to start the JVM.
if [ -n "$JAVA_HOME" ] ; then
    if [ -x "$JAVA_HOME/jre/sh/java" ] ; then
        # IBM's JDK on AIX uses strange locations for the executables
        JAVACMD="$JAVA_HOME/jre/sh/java"
    else
        JAVACMD="$JAVA_HOME/bin/java"
    fi
    if [ ! -x "$JAVACMD" ] ; then
        die "ERROR: JAVA_HOME is set to an invalid directory: $JAVA_HOME

Please set the JAVA_HOME variable in your environment to match the
location of your Java installation."
    fi
else
    JAVACMD="java"
    which java >/dev/null 2>&1 || die "ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.

Please set the JAVA_HOME variable in your environment to match the
location of your Java installation."
fi

# Increase the maximum file descriptors if we can.
if [ "$cygwin" = "false" -a "$darwin" = "false" ] ; then
    MAX_FD_LIMIT=`ulimit -H -n`
    if [ $? -eq 0 ] ; then
        if [ "$MAX_FD" = "maximum" -o "$MAX_FD" = "max" ] ; then
            MAX_FD="$MAX_FD_LIMIT"
        fi
        ulimit -n $MAX_FD
        if [ $? -ne 0 ] ; then
            warn "Could not set maximum file descriptor limit: $MAX_FD"
        fi
    else
        warn "Could not query maximum file descriptor limit: $MAX_FD_LIMIT"
    fi
fi

# For Darwin, add options to specify how the application appears in the dock
if $darwin; then
    GRADLE_OPTS="$GRADLE_OPTS \"-Xdock:name=$APP_NAME\" \"-Xdock:icon=$APP_HOME/media/gradle.icns\""
fi

# For Cygwin, switch paths to Windows format before running java
if $cygwin ; then
    APP_HOME=`cygpath --path --mixed "$APP_HOME"`
    CLASSPATH=`cygpath --path --mixed "$CLASSPATH"`

    # We build the pattern for arguments to be converted via cygpath
    ROOTDIRSRAW=`find -L / -maxdepth 1 -mindepth 1 -type d 2>/dev/null`
    SEP=""
    for dir in $ROOTDIRSRAW ; do
        ROOTDIRS="$ROOTDIRS$SEP$dir"
        SEP="|"
    done
    OURCYGPATTERN="(^($ROOTDIRS))"
    # Add a user-defined pattern to the cygpath arguments
    if [ "$GRADLE_CYGPATTERN" != "" ] ; then
        OURCYGPATTERN="$OURCYGPATTERN|($GRADLE_CYGPATTERN)"
    fi
    # Now convert the arguments - kludge to limit ourselves to /bin/sh
    i=0
    for arg in "$@" ; do
        CHECK=`echo "$arg"|egrep -c "$OURCYGPATTERN" -`
        CHECK2=`echo "$arg"|egrep -c "^-"`                                 ### Determine if an option

        if [ $CHECK -ne 0 ] && [ $CHECK2 -eq 0 ] ; then                    ### Added a condition
            eval `echo args$i`=`cygpath --path --ignore --mixed "$arg"`
        else
            eval `echo args$i`="\"$arg\""
        fi
        i=$((i+1))
    done
    case $i in
        (0) set -- ;;
        (1) set -- "$args0" ;;
        (2) set -- "$args0" "$args1" ;;
        (3) set -- "$args0" "$args1" "$args2" ;;
        (4) set -- "$args0" "$args1" "$args2" "$args3" ;;
        (5) set -- "$args0" "$args1" "$args2" "$args3" "$args4" ;;
        (6) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" ;;
        (7) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" ;;
        (8) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" "$args7" ;;
        (9) set -- "$args0" "$args1" "$args2" "$args3" "$args4" "$args5" "$args6" "$args7" "$args8" ;;
    esac
fi

# Split up the JVM_OPTS And GRADLE_OPTS values into an array, following the shell quoting and substitution rules
function splitJvmOpts() {
    JVM_OPTS=("$@")
}
eval splitJvmOpts $DEFAULT_JVM_OPTS $JAVA_OPTS $GRADLE_OPTS
JVM_OPTS[${#JVM_OPTS[*]}]="-Dorg.gradle.appname=$APP_BASE_NAME"

exec "$JAVACMD" "${JVM_OPTS[@]}" -classpath "$CLASSPATH" org.gradle.wrapper.GradleWrapperMain "$@"
          destination: @if "%DEBUG%" == "" @echo off
@rem ##########################################################################
@rem
@rem  Gradle startup script for Windows
@rem
@rem ##########################################################################

@rem Set local scope for the variables with windows NT shell
if "%OS%"=="Windows_NT" setlocal

@rem Add default JVM options here. You can also use JAVA_OPTS and GRADLE_OPTS to pass JVM options to this script.
set DEFAULT_JVM_OPTS=

set DIRNAME=%~dp0
if "%DIRNAME%" == "" set DIRNAME=.
set APP_BASE_NAME=%~n0
set APP_HOME=%DIRNAME%

@rem Find java.exe
if defined JAVA_HOME goto findJavaFromJavaHome

set JAVA_EXE=java.exe
%JAVA_EXE% -version >NUL 2>&1
if "%ERRORLEVEL%" == "0" goto init

echo.
echo ERROR: JAVA_HOME is not set and no 'java' command could be found in your PATH.
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:findJavaFromJavaHome
set JAVA_HOME=%JAVA_HOME:"=%
set JAVA_EXE=%JAVA_HOME%/bin/java.exe

if exist "%JAVA_EXE%" goto init

echo.
echo ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%
echo.
echo Please set the JAVA_HOME variable in your environment to match the
echo location of your Java installation.

goto fail

:init
@rem Get command-line arguments, handling Windowz variants

if not "%OS%" == "Windows_NT" goto win9xME_args
if "%@eval[2+2]" == "4" goto 4NT_args

:win9xME_args
@rem Slurp the command line arguments.
set CMD_LINE_ARGS=
set _SKIP=2

:win9xME_args_slurp
if "x%~1" == "x" goto execute

set CMD_LINE_ARGS=%*
goto execute

:4NT_args
@rem Get arguments from the 4NT Shell from JP Software
set CMD_LINE_ARGS=%$

:execute
@rem Setup the command line

set CLASSPATH=%APP_HOME%\gradle\wrapper\gradle-wrapper.jar

@rem Execute Gradle
"%JAVA_EXE%" %DEFAULT_JVM_OPTS% %JAVA_OPTS% %GRADLE_OPTS% "-Dorg.gradle.appname=%APP_BASE_NAME%" -classpath "%CLASSPATH%" org.gradle.wrapper.GradleWrapperMain %CMD_LINE_ARGS%

:end
@rem End local scope for the variables with windows NT shell
if "%ERRORLEVEL%"=="0" goto mainEnd

:fail
rem Set variable GRADLE_EXIT_CONSOLE if you need the _script_ return code instead of
rem the _cmd.exe /c_ return code!
if  not "" == "%GRADLE_EXIT_CONSOLE%" exit 1
exit /b 1

:mainEnd
if "%OS%"=="Windows_NT" endlocal

:omega
      - name: cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0026 OLD) # allow use of the LOCATION target property

# store the current source directory for future use
set(QT_ANDROID_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

# check the JAVA_HOME environment variable
# (I couldn't find a way to set it from this script, it has to be defined outside)
set(JAVA_HOME $ENV{JAVA_HOME})
if(NOT JAVA_HOME)
    message(FATAL_ERROR "The JAVA_HOME environment variable is not set. Please set it to the root directory of the JDK.")
endif()

# make sure that the Android toolchain is used
if(NOT ANDROID)
    message(FATAL_ERROR "Trying to use the CMake Android package without the Android toolchain. Please use the provided toolchain (toolchain/android.toolchain.cmake)")
endif()

# find the Qt root directory
if(NOT Qt5Core_DIR)
    find_package(Qt5Core REQUIRED)
endif()
get_filename_component(QT_ANDROID_QT_ROOT "${Qt5Core_DIR}/../../.." ABSOLUTE)
message(STATUS "Found Qt for Android: ${QT_ANDROID_QT_ROOT}")

# find the Android SDK
if(NOT QT_ANDROID_SDK_ROOT)
    set(QT_ANDROID_SDK_ROOT $ENV{ANDROID_SDK})
    if(NOT QT_ANDROID_SDK_ROOT)
        message(FATAL_ERROR "Could not find the Android SDK. Please set either the ANDROID_SDK environment variable, or the QT_ANDROID_SDK_ROOT CMake variable to the root directory of the Android SDK")
    endif()
endif()
string(REPLACE "\\" "/" QT_ANDROID_SDK_ROOT ${QT_ANDROID_SDK_ROOT}) # androiddeployqt doesn't like backslashes in paths
message(STATUS "Found Android SDK: ${QT_ANDROID_SDK_ROOT}")

# find the Android NDK
if(NOT QT_ANDROID_NDK_ROOT)
    set(QT_ANDROID_NDK_ROOT $ENV{ANDROID_NDK})
    if(NOT QT_ANDROID_NDK_ROOT)
        set(QT_ANDROID_NDK_ROOT ${ANDROID_NDK})
        if(NOT QT_ANDROID_NDK_ROOT)
        message(FATAL_ERROR "Could not find the Android NDK. Please set either the ANDROID_NDK environment or CMake variable, or the QT_ANDROID_NDK_ROOT CMake variable to the root directory of the Android NDK")
        endif()
    endif()
endif()
string(REPLACE "\\" "/" QT_ANDROID_NDK_ROOT ${QT_ANDROID_NDK_ROOT}) # androiddeployqt doesn't like backslashes in paths
message(STATUS "Found Android NDK: ${QT_ANDROID_NDK_ROOT}")

include(CMakeParseArguments)

# define a macro to create an Android APK target
#
# example:
# add_qt_android_apk(my_app_apk my_app
#     NAME "My App"
#     VERSION_CODE 12
#     PACKAGE_NAME "org.mycompany.myapp"
#     PACKAGE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/my-android-sources
#     BUILDTOOLS_REVISION "23.0.3"
#     KEYSTORE ${CMAKE_CURRENT_LIST_DIR}/mykey.keystore myalias
#     KEYSTORE_PASSWORD xxxx
#     DEPENDS a_linked_target "path/to/a_linked_library.so" ...
#     INSTALL
#)
# 
macro(add_qt_android_apk TARGET SOURCE_TARGET)

    # parse the macro arguments
    cmake_parse_arguments(ARG "INSTALL" "NAME;VERSION_CODE;PACKAGE_NAME;PACKAGE_SOURCES;KEYSTORE_PASSWORD;BUILDTOOLS_REVISION" "DEPENDS;KEYSTORE" ${ARGN})

    # extract the full path of the source target binary
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        get_property(QT_ANDROID_APP_PATH TARGET ${SOURCE_TARGET} PROPERTY DEBUG_LOCATION)
    else()
        get_property(QT_ANDROID_APP_PATH TARGET ${SOURCE_TARGET} PROPERTY LOCATION)
    endif()

    # define the application name
    if(ARG_NAME)
        set(QT_ANDROID_APP_NAME ${ARG_NAME})
    else()
        set(QT_ANDROID_APP_NAME ${SOURCE_TARGET})
    endif()

    # define the application package name
    if(ARG_PACKAGE_NAME)
        set(QT_ANDROID_APP_PACKAGE_NAME ${ARG_PACKAGE_NAME})
    else()
        set(QT_ANDROID_APP_PACKAGE_NAME org.qtproject.${SOURCE_TARGET})
    endif()

    # set the Android SDK build-tools revision
    if(ARG_BUILDTOOLS_REVISION)
        set(QT_ANDROID_SDK_BUILDTOOLS_REVISION ${ARG_BUILDTOOLS_REVISION})
    else()
        set(QT_ANDROID_SDK_BUILDTOOLS_REVISION "")
    endif()

    # define the application source package directory
    if(ARG_PACKAGE_SOURCES)
        set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT ${ARG_PACKAGE_SOURCES})
    else()
        # get version code from arguments, or generate a fixed one if not provided
        set(QT_ANDROID_APP_VERSION_CODE ${ARG_VERSION_CODE})
        if(NOT QT_ANDROID_APP_VERSION_CODE)
            set(QT_ANDROID_APP_VERSION_CODE 1)
        endif()

        # try to extract the app version from the target properties, or use the version code if not provided
        get_property(QT_ANDROID_APP_VERSION TARGET ${SOURCE_TARGET} PROPERTY VERSION)
        if(NOT QT_ANDROID_APP_VERSION)
            set(QT_ANDROID_APP_VERSION ${QT_ANDROID_APP_VERSION_CODE})
        endif()

        # create a subdirectory for the extra package sources
        set(QT_ANDROID_APP_PACKAGE_SOURCE_ROOT "${CMAKE_CURRENT_BINARY_DIR}/package")

        # generate a manifest from the template
        configure_file(${QT_ANDROID_SOURCE_DIR}/AndroidManifest.xml.in ${QT_ANDROID_APP_PACKAGE_SOURCE_ROOT}/AndroidManifest.xml @ONLY)
    endif()

    # set the list of dependant libraries
    if(ARG_DEPENDS)
        foreach(LIB ${ARG_DEPENDS})
            if(TARGET ${LIB})
                # item is a CMake target, extract the library path
                if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                    get_property(LIB_PATH TARGET ${LIB} PROPERTY DEBUG_LOCATION)
                else()
                    get_property(LIB_PATH TARGET ${LIB} PROPERTY LOCATION)
                endif()
                set(LIB ${LIB_PATH})
            endif()
        if(EXTRA_LIBS)
            set(EXTRA_LIBS "${EXTRA_LIBS},${LIB}")
        else()
            set(EXTRA_LIBS "${LIB}")
        endif()
        endforeach()
        set(QT_ANDROID_APP_EXTRA_LIBS "\"android-extra-libs\": \"${EXTRA_LIBS}\",")
    endif()

    # make sure that the output directory for the Android package exists
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI})

    # create the configuration file that will feed androiddeployqt
    configure_file(${QT_ANDROID_SOURCE_DIR}/qtdeploy.json.in ${CMAKE_CURRENT_BINARY_DIR}/qtdeploy.json @ONLY)

    # check if the apk must be signed
    if(ARG_KEYSTORE)
        set(SIGN_OPTIONS --release --sign ${ARG_KEYSTORE} --tsa http://timestamp.digicert.com)
        if(ARG_KEYSTORE_PASSWORD)
            set(SIGN_OPTIONS ${SIGN_OPTIONS} --storepass ${ARG_KEYSTORE_PASSWORD})
        endif()
    endif()

    # check if the apk must be installed to the device
    if(ARG_INSTALL)
        set(INSTALL_OPTIONS --reinstall)
    endif()

    # specify the Android API level
    if(ANDROID_NATIVE_API_LEVEL)
        set(TARGET_LEVEL_OPTIONS --android-platform android-${ANDROID_NATIVE_API_LEVEL})
    endif()

    # create a custom command that will run the androiddeployqt utility to prepare the Android package
    add_custom_target(
        ${TARGET}
        ALL
        DEPENDS ${SOURCE_TARGET}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI} # it seems that recompiled libraries are not copied if we don't remove them first
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT_ANDROID_APP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}
        COMMAND ${QT_ANDROID_QT_ROOT}/bin/androiddeployqt --verbose --output ${CMAKE_CURRENT_BINARY_DIR} --input ${CMAKE_CURRENT_BINARY_DIR}/qtdeploy.json --gradle ${TARGET_LEVEL_OPTIONS} ${INSTALL_OPTIONS} ${SIGN_OPTIONS}
    )

endmacro()
        uses: 
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        include(CTest)
        enable_testing()
        add_definitions(-DUNITTEST_BUILD)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # clang and AppleClang
        add_compile_options(
                -Wall
                -Wextra
                -Wno-address-of-packed-member # ignore for mavlink
        )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
                add_compile_options(-fdiagnostics-color=always)
        endif()

        add_compile_options(
                -Wall
                -Wextra
        )
elseif (WIN32)
        add_definitions(-D_USE_MATH_DEFINES)
        add_compile_options(
                /wd4244 # warning C4244: '=': conversion from 'double' to 'float', possible loss of data
    )
endif()

  # Deployment job
  deploy: find_package(Git)

if(NOT GIT_FOUND OR NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
    return()
endif()

# Update submodules as needed
option(GIT_SUBMODULE "Check submodules during build" ON)
if(NOT GIT_SUBMODULE)
    return()
endif()

message(STATUS "Submodule update")
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                RESULT_VARIABLE GIT_SUBMODULE_RESULT)
if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
    message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMODULE_RESULT}, please checkout submodules")
endif()

# Fetch the necessary git variables
execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --tags
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)
    environment: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>English</string>
    <key>CFBundleExecutable</key>
    <string>${MACOSX_BUNDLE_EXECUTABLE_NAME}</string>
    <key>CFBundleGetInfoString</key>
    <string>${MACOSX_BUNDLE_INFO_STRING}</string>
    <key>CFBundleIconFile</key>
    <string>${MACOSX_BUNDLE_ICON_FILE}</string>
    <key>CFBundleIdentifier</key>
    <string>${MACOSX_BUNDLE_GUI_IDENTIFIER}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleLongVersionString</key>
    <string>${MACOSX_BUNDLE_LONG_VERSION_STRING}</string>
    <key>CFBundleName</key>
    <string>${MACOSX_BUNDLE_BUNDLE_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>${MACOSX_BUNDLE_SHORT_VERSION_STRING}</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>CFBundleVersion</key>
    <string>${MACOSX_BUNDLE_BUNDLE_VERSION}</string>
    <key>CSResourcesFileMapped</key>
    <true/>
    <key>LSRequiresCarbon</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>${MACOSX_BUNDLE_COPYRIGHT}</string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
      name: 
if(LINUX)
        # TODO: investigate https://github.com/probonopd/linuxdeployqt

        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/release/package/QGroundControl.AppImage
                COMMAND ${CMAKE_SOURCE_DIR}/deploy/create_linux_appimage.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/release/package;
                DEPENDS QGroundControl
                USES_TERMINAL
        )
        add_custom_target(appimage DEPENDS ${CMAKE_BINARY_DIR}/release/package/QGroundControl.AppImage)

elseif(APPLE)

        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

        add_custom_command(TARGET QGroundControl
                POST_BUILD
                COMMAND
                        ${MACDEPLOYQT_EXECUTABLE} $<TARGET_FILE_DIR:QGroundControl>/../.. -appstore-compliant -qmldir=${CMAKE_SOURCE_DIR}/src
                COMMAND
                        rsync -a ${CMAKE_SOURCE_DIR}/libs/Frameworks $<TARGET_FILE_DIR:QGroundControl>/../../Contents/
                COMMAND
                        ${CMAKE_INSTALL_NAME_TOOL} -change "@rpath/SDL2.framework/Versions/A/SDL2" "@executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2" $<TARGET_FILE:QGroundControl>
        )

        set_target_properties(QGroundControl PROPERTIES MACOSX_BUNDLE YES)

elseif(WIN32)
        if(MSVC) # Check if we are using the Visual Studio compiler
                set_target_properties(${PROJECT_NAME} PROPERTIES
                        WIN32_EXECUTABLE YES
                        LINK_FLAGS "/ENTRY:mainCRTStartup"
                )
        endif()

        # deploy
        include(Windeployqt)
        windeployqt(QGroundControl "QGroundControl-installer.exe")

elseif(ANDROID)
        include(AddQtAndroidApk)
        add_qt_android_apk(QGroundControl.apk QGroundControl
                PACKAGE_NAME "io.mavlink.qgroundcontrol"
                #KEYSTORE ${CMAKE_CURRENT_LIST_DIR}/mykey.keystore myalias
                #KEYSTORE_PASSWORD xxxxx
        )
endif()
      url: if(DEFINED ENV{QT_VERSION})
        set(QT_VERSION $ENV{QT_VERSION})
endif()

if(NOT QT_VERSION)
        # try Qt 5.12.0 if none specified, last LTS.
        set(QT_VERSION "5.12.5")
endif()

if(DEFINED ENV{QT_MKSPEC})
        set(QT_MKSPEC $ENV{QT_MKSPEC})
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
        set(LINUX TRUE)
endif()

if(NOT QT_MKSPEC)
        if(APPLE)
                set(QT_MKSPEC clang_64)
        elseif(LINUX)
                set(QT_MKSPEC gcc_64)
        elseif(WIN32)
                set(QT_MKSPEC msvc2017_64)
                #set(QT_MKSPEC winrt_x64_msvc2017)
        endif()
endif()

set(QT_LIBRARY_HINTS
                $ENV{HOME}/Qt/${QT_VERSION}/${QT_MKSPEC}
                $ENV{QT_PATH}/${QT_VERSION}/${QT_MKSPEC}
                C:/Qt
)
    runs-on: # The MIT License (MIT)
#
# Copyright (c) 2017 
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

find_package(Qt5Core REQUIRED)

# Retrieve the absolute path to qmake and then use that path to find
# the windeployqt binary
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

# Running this with MSVC 2015 requires CMake 3.6+
if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
        AND CMAKE_VERSION VERSION_LESS "3.6")
    message(WARNING "Deploying with MSVC 2015+ requires CMake 3.6+")
endif()

# Add commands that copy the Qt runtime to the target's output directory after
# build and install the Qt runtime to the specified directory
function(windeployqt target directory)

    # Run windeployqt immediately after build
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                --verbose 0
                --no-compiler-runtime
                --no-angle
                --no-opengl-sw
                \"$<TARGET_FILE:${target}>\"
    )

    # install(CODE ...) doesn't support generator expressions, but
    # file(GENERATE ...) does - store the path in a file
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}_path"
        CONTENT "$<TARGET_FILE:${target}>"
    )

    # Before installation, run a series of commands that copy each of the Qt
    # runtime files to the appropriate directory for installation
    install(CODE
        "
        file(READ \"${CMAKE_CURRENT_BINARY_DIR}/${target}_path\" _file)
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E
                env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
                    --dry-run
                    --no-compiler-runtime
                    --no-angle
                    --no-opengl-sw
                    --list mapping
                    \${_file}
            OUTPUT_VARIABLE _output
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        separate_arguments(_files WINDOWS_COMMAND \${_output})
        while(_files)
            list(GET _files 0 _src)
            list(GET _files 1 _dest)
            execute_process(
                COMMAND \"${CMAKE_COMMAND}\" -E
                    copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/${directory}/\${_dest}\"
            )
            list(REMOVE_AT _files 0 1)
        endwhile()
        "
    )

    # windeployqt doesn't work correctly with the system runtime libraries,
    # so we fall back to one of CMake's own modules for copying them over
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
    foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        get_filename_component(filename "${lib}" NAME)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "${lib}" \"$<TARGET_FILE_DIR:${target}>\"
        )
    endforeach()

endfunction()

mark_as_advanced(WINDEPLOYQT_EXECUTABLE)
    needs: # All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1.  Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
# 2.  Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# 3.  Neither the name of the copyright holder nor the names of its
#     contributors may be used to endorse or promote products derived from this
#     software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# ------------------------------------------------------------------------------
#  Android CMake toolchain file, for use with the Android NDK r5-r10d
#  Requires cmake 2.6.3 or newer (2.8.9 or newer is recommended).
#  See home page: https://github.com/taka-no-me/android-cmake
#
#  Usage Linux:
#   $ export ANDROID_NDK=/absolute/path/to/the/android-ndk
#   $ mkdir build && cd build
#   $ cmake -DCMAKE_TOOLCHAIN_FILE=path/to/the/android.toolchain.cmake ..
#   $ make -j8
#
#  Usage Windows:
#     You need native port of make to build your project.
#     Android NDK r7 (and newer) already has make.exe on board.
#     For older NDK you have to install it separately.
#     For example, this one: http://gnuwin32.sourceforge.net/packages/make.htm
#
#   $ SET ANDROID_NDK=C:\absolute\path\to\the\android-ndk
#   $ mkdir build && cd build
#   $ cmake.exe -G"MinGW Makefiles"
#       -DCMAKE_TOOLCHAIN_FILE=path\to\the\android.toolchain.cmake
#       -DCMAKE_MAKE_PROGRAM="%ANDROID_NDK%\prebuilt\windows\bin\make.exe" ..
#   $ cmake.exe --build .
#
#
#  Options (can be set as cmake parameters: -D<option_name>=<value>):
#    ANDROID_NDK=/opt/android-ndk - path to the NDK root.
#      Can be set as environment variable. Can be set only at first cmake run.
#
#    ANDROID_ABI=armeabi-v7a - specifies the target Application Binary
#      Interface (ABI). This option nearly matches to the APP_ABI variable
#      used by ndk-build tool from Android NDK.
#
#      Possible targets are:
#        "armeabi" - ARMv5TE based CPU with software floating point operations
#        "armeabi-v7a" - ARMv7 based devices with hardware FPU instructions
#            this ABI target is used by default
#        "armeabi-v7a with NEON" - same as armeabi-v7a, but
#            sets NEON as floating-point unit
#        "armeabi-v7a with VFPV3" - same as armeabi-v7a, but
#            sets VFPV3 as floating-point unit (has 32 registers instead of 16)
#        "armeabi-v6 with VFP" - tuned for ARMv6 processors having VFP
#        "x86" - IA-32 instruction set
#        "mips" - MIPS32 instruction set
#
#      64-bit ABIs for NDK r10 and newer:
#        "arm64-v8a" - ARMv8 AArch64 instruction set
#        "x86_64" - Intel64 instruction set (r1)
#        "mips64" - MIPS64 instruction set (r6)
#
#    ANDROID_NATIVE_API_LEVEL=android-8 - level of Android API compile for.
#      Option is read-only when standalone toolchain is used.
#      Note: building for "android-L" requires explicit configuration.
#
#    ANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9 - the name of compiler
#      toolchain to be used. The list of possible values depends on the NDK
#      version. For NDK r10c the possible values are:
#
#        * aarch64-linux-android-4.9
#        * aarch64-linux-android-clang3.4
#        * aarch64-linux-android-clang3.5
#        * arm-linux-androideabi-4.6
#        * arm-linux-androideabi-4.8
#        * arm-linux-androideabi-4.9 (default)
#        * arm-linux-androideabi-clang3.4
#        * arm-linux-androideabi-clang3.5
#        * mips64el-linux-android-4.9
#        * mips64el-linux-android-clang3.4
#        * mips64el-linux-android-clang3.5
#        * mipsel-linux-android-4.6
#        * mipsel-linux-android-4.8
#        * mipsel-linux-android-4.9
#        * mipsel-linux-android-clang3.4
#        * mipsel-linux-android-clang3.5
#        * x86-4.6
#        * x86-4.8
#        * x86-4.9
#        * x86-clang3.4
#        * x86-clang3.5
#        * x86_64-4.9
#        * x86_64-clang3.4
#        * x86_64-clang3.5
#
#    ANDROID_FORCE_ARM_BUILD=OFF - set ON to generate 32-bit ARM instructions
#      instead of Thumb. Is not available for "armeabi-v6 with VFP"
#      (is forced to be ON) ABI.
#
#    ANDROID_NO_UNDEFINED=ON - set ON to show all undefined symbols as linker
#      errors even if they are not used.
#
#    ANDROID_SO_UNDEFINED=OFF - set ON to allow undefined symbols in shared
#      libraries. Automatically turned for NDK r5x and r6x due to GLESv2
#      problems.
#
#    ANDROID_STL=gnustl_static - specify the runtime to use.
#
#      Possible values are:
#        none           -> Do not configure the runtime.
#        system         -> Use the default minimal system C++ runtime library.
#                          Implies -fno-rtti -fno-exceptions.
#                          Is not available for standalone toolchain.
#        system_re      -> Use the default minimal system C++ runtime library.
#                          Implies -frtti -fexceptions.
#                          Is not available for standalone toolchain.
#        gabi++_static  -> Use the GAbi++ runtime as a static library.
#                          Implies -frtti -fno-exceptions.
#                          Available for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        gabi++_shared  -> Use the GAbi++ runtime as a shared library.
#                          Implies -frtti -fno-exceptions.
#                          Available for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        stlport_static -> Use the STLport runtime as a static library.
#                          Implies -fno-rtti -fno-exceptions for NDK before r7.
#                          Implies -frtti -fno-exceptions for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        stlport_shared -> Use the STLport runtime as a shared library.
#                          Implies -fno-rtti -fno-exceptions for NDK before r7.
#                          Implies -frtti -fno-exceptions for NDK r7 and newer.
#                          Is not available for standalone toolchain.
#        gnustl_static  -> Use the GNU STL as a static library.
#                          Implies -frtti -fexceptions.
#        gnustl_shared  -> Use the GNU STL as a shared library.
#                          Implies -frtti -fno-exceptions.
#                          Available for NDK r7b and newer.
#                          Silently degrades to gnustl_static if not available.
#
#    ANDROID_STL_FORCE_FEATURES=ON - turn rtti and exceptions support based on
#      chosen runtime. If disabled, then the user is responsible for settings
#      these options.
#
#  What?:
#    android-cmake toolchain searches for NDK/toolchain in the following order:
#      ANDROID_NDK - cmake parameter
#      ANDROID_NDK - environment variable
#      ANDROID_STANDALONE_TOOLCHAIN - cmake parameter
#      ANDROID_STANDALONE_TOOLCHAIN - environment variable
#      ANDROID_NDK - default locations
#      ANDROID_STANDALONE_TOOLCHAIN - default locations
#
#    Make sure to do the following in your scripts:
#      SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${my_cxx_flags}" )
#      SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${my_cxx_flags}" )
#    The flags will be prepopulated with critical flags, so don't loose them.
#    Also be aware that toolchain also sets configuration-specific compiler
#    flags and linker flags.
#
#    ANDROID and BUILD_ANDROID will be set to true, you may test any of these
#    variables to make necessary Android-specific configuration changes.
#
#    Also ARMEABI or ARMEABI_V7A or X86 or MIPS or ARM64_V8A or X86_64 or MIPS64
#    will be set true, mutually exclusive. NEON option will be set true
#    if VFP is set to NEON.
#
# ------------------------------------------------------------------------------

cmake_minimum_required( VERSION 2.6.3 )

if( DEFINED CMAKE_CROSSCOMPILING )
 # subsequent toolchain loading is not really needed
 return()
endif()

if( CMAKE_TOOLCHAIN_FILE )
 # touch toolchain variable to suppress "unused variable" warning
endif()

# inherit settings in recursive loads
get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if( _CMAKE_IN_TRY_COMPILE )
 include( "${CMAKE_CURRENT_SOURCE_DIR}/../android.toolchain.config.cmake" OPTIONAL )
endif()

# this one is important
if( CMAKE_VERSION VERSION_GREATER "3.0.99" )
 set( CMAKE_SYSTEM_NAME Android )
else()
 set( CMAKE_SYSTEM_NAME Linux )
endif()

# this one not so much
set( CMAKE_SYSTEM_VERSION 1 )

# rpath makes low sense for Android
set( CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "" )
set( CMAKE_SKIP_RPATH TRUE CACHE BOOL "If set, runtime paths are not added when using shared libraries." )

# NDK search paths
set( ANDROID_SUPPORTED_NDK_VERSIONS ${ANDROID_EXTRA_NDK_VERSIONS} -r10d -r10c -r10b -r10 -r9d -r9c -r9b -r9 -r8e -r8d -r8c -r8b -r8 -r7c -r7b -r7 -r6b -r6 -r5c -r5b -r5 "" )
if( NOT DEFINED ANDROID_NDK_SEARCH_PATHS )
 if( CMAKE_HOST_WIN32 )
  file( TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ANDROID_NDK_SEARCH_PATHS )
  set( ANDROID_NDK_SEARCH_PATHS "${ANDROID_NDK_SEARCH_PATHS}" "$ENV{SystemDrive}/NVPACK" )
 else()
  file( TO_CMAKE_PATH "$ENV{HOME}" ANDROID_NDK_SEARCH_PATHS )
  set( ANDROID_NDK_SEARCH_PATHS /opt "${ANDROID_NDK_SEARCH_PATHS}/NVPACK" )
 endif()
endif()
if( NOT DEFINED ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH )
 set( ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH /opt/android-toolchain )
endif()

# known ABIs
set( ANDROID_SUPPORTED_ABIS_arm "armeabi-v7a;armeabi;armeabi-v7a with NEON;armeabi-v7a with VFPV3;armeabi-v6 with VFP" )
set( ANDROID_SUPPORTED_ABIS_arm64 "arm64-v8a" )
set( ANDROID_SUPPORTED_ABIS_x86 "x86" )
set( ANDROID_SUPPORTED_ABIS_x86_64 "x86_64" )
set( ANDROID_SUPPORTED_ABIS_mips "mips" )
set( ANDROID_SUPPORTED_ABIS_mips64 "mips64" )

# API level defaults
set( ANDROID_DEFAULT_NDK_API_LEVEL 8 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_arm64 21 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_x86 9 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_x86_64 21 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_mips 9 )
set( ANDROID_DEFAULT_NDK_API_LEVEL_mips64 21 )


macro( __LIST_FILTER listvar regex )
  if( ${listvar} )
    foreach( __val ${${listvar}} )
      if( __val MATCHES "${regex}" )
        list( REMOVE_ITEM ${listvar} "${__val}" )
      endif()
    endforeach()
  endif()
endmacro()

macro( __INIT_VARIABLE var_name )
  set( __test_path 0 )
  foreach( __var ${ARGN} )
    if( __var STREQUAL "PATH" )
      set( __test_path 1 )
      break()
    endif()
  endforeach()

  if( __test_path AND NOT EXISTS "${${var_name}}" )
    unset( ${var_name} CACHE )
  endif()

  if( " ${${var_name}}" STREQUAL " " )
    set( __values 0 )
    foreach( __var ${ARGN} )
      if( __var STREQUAL "VALUES" )
        set( __values 1 )
      elseif( NOT __var STREQUAL "PATH" )
        if( __var MATCHES "^ENV_.*$" )
          string( REPLACE "ENV_" "" __var "${__var}" )
          set( __value "$ENV{${__var}}" )
        elseif( DEFINED ${__var} )
          set( __value "${${__var}}" )
        elseif( __values )
          set( __value "${__var}" )
        else()
          set( __value "" )
        endif()

        if( NOT " ${__value}" STREQUAL " " AND (NOT __test_path OR EXISTS "${__value}") )
          set( ${var_name} "${__value}" )
          break()
        endif()
      endif()
    endforeach()
    unset( __value )
    unset( __values )
  endif()

  if( __test_path )
    file( TO_CMAKE_PATH "${${var_name}}" ${var_name} )
  endif()
  unset( __test_path )
endmacro()

macro( __DETECT_NATIVE_API_LEVEL _var _path )
  set( __ndkApiLevelRegex "^[\t ]*#define[\t ]+__ANDROID_API__[\t ]+([0-9]+)[\t ]*.*$" )
  file( STRINGS ${_path} __apiFileContent REGEX "${__ndkApiLevelRegex}" )
  if( NOT __apiFileContent )
    message( SEND_ERROR "Could not get Android native API level. Probably you have specified invalid level value, or your copy of NDK/toolchain is broken." )
  endif()
  string( REGEX REPLACE "${__ndkApiLevelRegex}" "\\1" ${_var} "${__apiFileContent}" )
  unset( __apiFileContent )
  unset( __ndkApiLevelRegex )
endmacro()

macro( __DETECT_TOOLCHAIN_MACHINE_NAME _var _root )
 if( EXISTS "${_root}" )
    file( GLOB __gccExePath RELATIVE "${_root}/bin/" "${_root}/bin/*-gcc${TOOL_OS_SUFFIX}" )
    __LIST_FILTER( __gccExePath "^[.].*" )
    list( LENGTH __gccExePath __gccExePathsCount )
    if( NOT __gccExePathsCount EQUAL 1  AND NOT _CMAKE_IN_TRY_COMPILE )
      message( WARNING "Could not determine machine name for compiler from ${_root}" )
      set( ${_var} "" )
    else()
      get_filename_component( __gccExeName "${__gccExePath}" NAME_WE )
      string( REPLACE "-gcc" "" ${_var} "${__gccExeName}" )
    endif()
    unset( __gccExePath )
    unset( __gccExePathsCount )
    unset( __gccExeName )
  else()
    set( ${_var} "" )
  endif()
endmacro()


# fight against cygwin
set( ANDROID_FORBID_SYGWIN TRUE CACHE BOOL "Prevent cmake from working under cygwin and using cygwin tools")
mark_as_advanced( ANDROID_FORBID_SYGWIN )
if( ANDROID_FORBID_SYGWIN )
 if( CYGWIN )
  message( FATAL_ERROR "Android NDK and android-cmake toolchain are not welcome Cygwin. It is unlikely that this cmake toolchain will work under cygwin. But if you want to try then you can set cmake variable ANDROID_FORBID_SYGWIN to FALSE and rerun cmake." )
 endif()

 if( CMAKE_HOST_WIN32 )
  # remove cygwin from PATH
  set( __new_path "$ENV{PATH}")
  __LIST_FILTER( __new_path "cygwin" )
  set(ENV{PATH} "${__new_path}")
  unset(__new_path)
 endif()
endif()


# detect current host platform
if( NOT DEFINED ANDROID_NDK_HOST_X64 AND (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "amd64|x86_64|AMD64" OR CMAKE_HOST_APPLE) )
 set( ANDROID_NDK_HOST_X64 1 CACHE BOOL "Try to use 64-bit compiler toolchain" )
 mark_as_advanced( ANDROID_NDK_HOST_X64 )
endif()

set( TOOL_OS_SUFFIX "" )
if( CMAKE_HOST_APPLE )
 set( ANDROID_NDK_HOST_SYSTEM_NAME "darwin-x86_64" )
 set( ANDROID_NDK_HOST_SYSTEM_NAME2 "darwin-x86" )
elseif( CMAKE_HOST_WIN32 )
 set( ANDROID_NDK_HOST_SYSTEM_NAME "windows-x86_64" )
 set( ANDROID_NDK_HOST_SYSTEM_NAME2 "windows" )
 set( TOOL_OS_SUFFIX ".exe" )
elseif( CMAKE_HOST_UNIX )
 set( ANDROID_NDK_HOST_SYSTEM_NAME "linux-x86_64" )
 set( ANDROID_NDK_HOST_SYSTEM_NAME2 "linux-x86" )
else()
 message( FATAL_ERROR "Cross-compilation on your platform is not supported by this cmake toolchain" )
endif()

if( NOT ANDROID_NDK_HOST_X64 )
 set( ANDROID_NDK_HOST_SYSTEM_NAME ${ANDROID_NDK_HOST_SYSTEM_NAME2} )
endif()

# see if we have path to Android NDK
if( NOT ANDROID_NDK AND NOT ANDROID_STANDALONE_TOOLCHAIN )
  __INIT_VARIABLE( ANDROID_NDK PATH ENV_ANDROID_NDK )
endif()
if( NOT ANDROID_NDK )
 # see if we have path to Android standalone toolchain
 __INIT_VARIABLE( ANDROID_STANDALONE_TOOLCHAIN PATH ENV_ANDROID_STANDALONE_TOOLCHAIN )

 if( NOT ANDROID_STANDALONE_TOOLCHAIN )
  #try to find Android NDK in one of the the default locations
  set( __ndkSearchPaths )
  foreach( __ndkSearchPath ${ANDROID_NDK_SEARCH_PATHS} )
   foreach( suffix ${ANDROID_SUPPORTED_NDK_VERSIONS} )
    list( APPEND __ndkSearchPaths "${__ndkSearchPath}/android-ndk${suffix}" )
   endforeach()
  endforeach()
  __INIT_VARIABLE( ANDROID_NDK PATH VALUES ${__ndkSearchPaths} )
  unset( __ndkSearchPaths )

  if( ANDROID_NDK )
   message( STATUS "Using default path for Android NDK: ${ANDROID_NDK}" )
   message( STATUS "  If you prefer to use a different location, please define a cmake or environment variable: ANDROID_NDK" )
  else()
   #try to find Android standalone toolchain in one of the the default locations
   __INIT_VARIABLE( ANDROID_STANDALONE_TOOLCHAIN PATH ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH )

   if( ANDROID_STANDALONE_TOOLCHAIN )
    message( STATUS "Using default path for standalone toolchain ${ANDROID_STANDALONE_TOOLCHAIN}" )
    message( STATUS "  If you prefer to use a different location, please define the variable: ANDROID_STANDALONE_TOOLCHAIN" )
   endif( ANDROID_STANDALONE_TOOLCHAIN )
  endif( ANDROID_NDK )
 endif( NOT ANDROID_STANDALONE_TOOLCHAIN )
endif( NOT ANDROID_NDK )

# remember found paths
if( ANDROID_NDK )
 get_filename_component( ANDROID_NDK "${ANDROID_NDK}" ABSOLUTE )
 set( ANDROID_NDK "${ANDROID_NDK}" CACHE INTERNAL "Path of the Android NDK" FORCE )
 set( BUILD_WITH_ANDROID_NDK True )
 if( EXISTS "${ANDROID_NDK}/RELEASE.TXT" )
  file( STRINGS "${ANDROID_NDK}/RELEASE.TXT" ANDROID_NDK_RELEASE_FULL LIMIT_COUNT 1 REGEX "r[0-9]+[a-z]?" )
  string( REGEX MATCH "r([0-9]+)([a-z]?)" ANDROID_NDK_RELEASE "${ANDROID_NDK_RELEASE_FULL}" )
 else()
  set( ANDROID_NDK_RELEASE "r1x" )
  set( ANDROID_NDK_RELEASE_FULL "unreleased" )
 endif()
 string( REGEX REPLACE "r([0-9]+)([a-z]?)" "\\1*1000" ANDROID_NDK_RELEASE_NUM "${ANDROID_NDK_RELEASE}" )
 string( FIND " abcdefghijklmnopqastuvwxyz" "${CMAKE_MATCH_2}" __ndkReleaseLetterNum )
 math( EXPR ANDROID_NDK_RELEASE_NUM "${ANDROID_NDK_RELEASE_NUM}+${__ndkReleaseLetterNum}" )
elseif( ANDROID_STANDALONE_TOOLCHAIN )
 get_filename_component( ANDROID_STANDALONE_TOOLCHAIN "${ANDROID_STANDALONE_TOOLCHAIN}" ABSOLUTE )
 # try to detect change
 if( CMAKE_AR )
  string( LENGTH "${ANDROID_STANDALONE_TOOLCHAIN}" __length )
  string( SUBSTRING "${CMAKE_AR}" 0 ${__length} __androidStandaloneToolchainPreviousPath )
  if( NOT __androidStandaloneToolchainPreviousPath STREQUAL ANDROID_STANDALONE_TOOLCHAIN )
   message( FATAL_ERROR "It is not possible to change path to the Android standalone toolchain on subsequent run." )
  endif()
  unset( __androidStandaloneToolchainPreviousPath )
  unset( __length )
 endif()
 set( ANDROID_STANDALONE_TOOLCHAIN "${ANDROID_STANDALONE_TOOLCHAIN}" CACHE INTERNAL "Path of the Android standalone toolchain" FORCE )
 set( BUILD_WITH_STANDALONE_TOOLCHAIN True )
else()
 list(GET ANDROID_NDK_SEARCH_PATHS 0 ANDROID_NDK_SEARCH_PATH)
 message( FATAL_ERROR "Could not find neither Android NDK nor Android standalone toolchain.
    You should either set an environment variable:
      export ANDROID_NDK=~/my-android-ndk
    or
      export ANDROID_STANDALONE_TOOLCHAIN=~/my-android-toolchain
    or put the toolchain or NDK in the default path:
      sudo ln -s ~/my-android-ndk ${ANDROID_NDK_SEARCH_PATH}/android-ndk
      sudo ln -s ~/my-android-toolchain ${ANDROID_STANDALONE_TOOLCHAIN_SEARCH_PATH}" )
endif()

# android NDK layout
if( BUILD_WITH_ANDROID_NDK )
 if( NOT DEFINED ANDROID_NDK_LAYOUT )
  # try to automatically detect the layout
  if( EXISTS "${ANDROID_NDK}/RELEASE.TXT")
   set( ANDROID_NDK_LAYOUT "RELEASE" )
  elseif( EXISTS "${ANDROID_NDK}/../../linux-x86/toolchain/" )
   set( ANDROID_NDK_LAYOUT "LINARO" )
  elseif( EXISTS "${ANDROID_NDK}/../../gcc/" )
   set( ANDROID_NDK_LAYOUT "ANDROID" )
  endif()
 endif()
 set( ANDROID_NDK_LAYOUT "${ANDROID_NDK_LAYOUT}" CACHE STRING "The inner layout of NDK" )
 mark_as_advanced( ANDROID_NDK_LAYOUT )
 if( ANDROID_NDK_LAYOUT STREQUAL "LINARO" )
  set( ANDROID_NDK_HOST_SYSTEM_NAME ${ANDROID_NDK_HOST_SYSTEM_NAME2} ) # only 32-bit at the moment
  set( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK}/../../${ANDROID_NDK_HOST_SYSTEM_NAME}/toolchain" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH  "" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH2 "" )
 elseif( ANDROID_NDK_LAYOUT STREQUAL "ANDROID" )
  set( ANDROID_NDK_HOST_SYSTEM_NAME ${ANDROID_NDK_HOST_SYSTEM_NAME2} ) # only 32-bit at the moment
  set( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK}/../../gcc/${ANDROID_NDK_HOST_SYSTEM_NAME}/arm" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH  "" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH2 "" )
 else() # ANDROID_NDK_LAYOUT STREQUAL "RELEASE"
  set( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK}/toolchains" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH  "/prebuilt/${ANDROID_NDK_HOST_SYSTEM_NAME}" )
  set( ANDROID_NDK_TOOLCHAINS_SUBPATH2 "/prebuilt/${ANDROID_NDK_HOST_SYSTEM_NAME2}" )
 endif()
 get_filename_component( ANDROID_NDK_TOOLCHAINS_PATH "${ANDROID_NDK_TOOLCHAINS_PATH}" ABSOLUTE )

 # try to detect change of NDK
 if( CMAKE_AR )
  string( LENGTH "${ANDROID_NDK_TOOLCHAINS_PATH}" __length )
  string( SUBSTRING "${CMAKE_AR}" 0 ${__length} __androidNdkPreviousPath )
  if( NOT __androidNdkPreviousPath STREQUAL ANDROID_NDK_TOOLCHAINS_PATH )
   message( FATAL_ERROR "It is not possible to change the path to the NDK on subsequent CMake run. You must remove all generated files from your build folder first.
   " )
  endif()
  unset( __androidNdkPreviousPath )
  unset( __length )
 endif()
endif()


# get all the details about standalone toolchain
if( BUILD_WITH_STANDALONE_TOOLCHAIN )
 __DETECT_NATIVE_API_LEVEL( ANDROID_SUPPORTED_NATIVE_API_LEVELS "${ANDROID_STANDALONE_TOOLCHAIN}/sysroot/usr/include/android/api-level.h" )
 set( ANDROID_STANDALONE_TOOLCHAIN_API_LEVEL ${ANDROID_SUPPORTED_NATIVE_API_LEVELS} )
 set( __availableToolchains "standalone" )
 __DETECT_TOOLCHAIN_MACHINE_NAME( __availableToolchainMachines "${ANDROID_STANDALONE_TOOLCHAIN}" )
 if( NOT __availableToolchainMachines )
  message( FATAL_ERROR "Could not determine machine name of your toolchain. Probably your Android standalone toolchain is broken." )
 endif()
 if( __availableToolchainMachines MATCHES x86_64 )
  set( __availableToolchainArchs "x86_64" )
 elseif( __availableToolchainMachines MATCHES i686 )
  set( __availableToolchainArchs "x86" )
 elseif( __availableToolchainMachines MATCHES aarch64 )
  set( __availableToolchainArchs "arm64" )
 elseif( __availableToolchainMachines MATCHES arm )
  set( __availableToolchainArchs "arm" )
 elseif( __availableToolchainMachines MATCHES mips64el )
  set( __availableToolchainArchs "mips64" )
 elseif( __availableToolchainMachines MATCHES mipsel )
  set( __availableToolchainArchs "mips" )
 endif()
 execute_process( COMMAND "${ANDROID_STANDALONE_TOOLCHAIN}/bin/${__availableToolchainMachines}-gcc${TOOL_OS_SUFFIX}" -dumpversion
                  OUTPUT_VARIABLE __availableToolchainCompilerVersions OUTPUT_STRIP_TRAILING_WHITESPACE )
 string( REGEX MATCH "[0-9]+[.][0-9]+([.][0-9]+)?" __availableToolchainCompilerVersions "${__availableToolchainCompilerVersions}" )
 if( EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/bin/clang${TOOL_OS_SUFFIX}" )
  list( APPEND __availableToolchains "standalone-clang" )
  list( APPEND __availableToolchainMachines ${__availableToolchainMachines} )
  list( APPEND __availableToolchainArchs ${__availableToolchainArchs} )
  list( APPEND __availableToolchainCompilerVersions ${__availableToolchainCompilerVersions} )
 endif()
endif()

macro( __GLOB_NDK_TOOLCHAINS __availableToolchainsVar __availableToolchainsLst __toolchain_subpath )
 foreach( __toolchain ${${__availableToolchainsLst}} )
  if( "${__toolchain}" MATCHES "-clang3[.][0-9]$" AND NOT EXISTS "${ANDROID_NDK_TOOLCHAINS_PATH}/${__toolchain}${__toolchain_subpath}" )
   SET( __toolchainVersionRegex "^TOOLCHAIN_VERSION[\t ]+:=[\t ]+(.*)$" )
   FILE( STRINGS "${ANDROID_NDK_TOOLCHAINS_PATH}/${__toolchain}/setup.mk" __toolchainVersionStr REGEX "${__toolchainVersionRegex}" )
   if( __toolchainVersionStr )
    string( REGEX REPLACE "${__toolchainVersionRegex}" "\\1" __toolchainVersionStr "${__toolchainVersionStr}" )
    string( REGEX REPLACE "-clang3[.][0-9]$" "-${__toolchainVersionStr}" __gcc_toolchain "${__toolchain}" )
   else()
    string( REGEX REPLACE "-clang3[.][0-9]$" "-4.6" __gcc_toolchain "${__toolchain}" )
   endif()
   unset( __toolchainVersionStr )
   unset( __toolchainVersionRegex )
  else()
   set( __gcc_toolchain "${__toolchain}" )
  endif()
  __DETECT_TOOLCHAIN_MACHINE_NAME( __machine "${ANDROID_NDK_TOOLCHAINS_PATH}/${__gcc_toolchain}${__toolchain_subpath}" )
  if( __machine )
   string( REGEX MATCH "[0-9]+[.][0-9]+([.][0-9x]+)?$" __version "${__gcc_toolchain}" )
   if( __machine MATCHES x86_64 )
    set( __arch "x86_64" )
   elseif( __machine MATCHES i686 )
    set( __arch "x86" )
   elseif( __machine MATCHES aarch64 )
    set( __arch "arm64" )
   elseif( __machine MATCHES arm )
    set( __arch "arm" )
   elseif( __machine MATCHES mips64el )
    set( __arch "mips64" )
   elseif( __machine MATCHES mipsel )
    set( __arch "mips" )
   else()
    set( __arch "" )
   endif()
   #message("machine: !${__machine}!\narch: !${__arch}!\nversion: !${__version}!\ntoolchain: !${__toolchain}!\n")
   if (__arch)
    list( APPEND __availableToolchainMachines "${__machine}" )
    list( APPEND __availableToolchainArchs "${__arch}" )
    list( APPEND __availableToolchainCompilerVersions "${__version}" )
    list( APPEND ${__availableToolchainsVar} "${__toolchain}" )
   endif()
  endif()
  unset( __gcc_toolchain )
 endforeach()
endmacro()

# get all the details about NDK
if( BUILD_WITH_ANDROID_NDK )
 file( GLOB ANDROID_SUPPORTED_NATIVE_API_LEVELS RELATIVE "${ANDROID_NDK}/platforms" "${ANDROID_NDK}/platforms/android-*" )
 string( REPLACE "android-" "" ANDROID_SUPPORTED_NATIVE_API_LEVELS "${ANDROID_SUPPORTED_NATIVE_API_LEVELS}" )
 set( __availableToolchains "" )
 set( __availableToolchainMachines "" )
 set( __availableToolchainArchs "" )
 set( __availableToolchainCompilerVersions "" )
 if( ANDROID_TOOLCHAIN_NAME AND EXISTS "${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_TOOLCHAIN_NAME}/" )
  # do not go through all toolchains if we know the name
  set( __availableToolchainsLst "${ANDROID_TOOLCHAIN_NAME}" )
  __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
  if( NOT __availableToolchains AND NOT ANDROID_NDK_TOOLCHAINS_SUBPATH STREQUAL ANDROID_NDK_TOOLCHAINS_SUBPATH2 )
   __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH2}" )
   if( __availableToolchains )
    set( ANDROID_NDK_TOOLCHAINS_SUBPATH ${ANDROID_NDK_TOOLCHAINS_SUBPATH2} )
   endif()
  endif()
 endif()
 if( NOT __availableToolchains )
  file( GLOB __availableToolchainsLst RELATIVE "${ANDROID_NDK_TOOLCHAINS_PATH}" "${ANDROID_NDK_TOOLCHAINS_PATH}/*" )
  if( __availableToolchains )
   list(SORT __availableToolchainsLst) # we need clang to go after gcc
  endif()
  __LIST_FILTER( __availableToolchainsLst "^[.]" )
  __LIST_FILTER( __availableToolchainsLst "llvm" )
  __LIST_FILTER( __availableToolchainsLst "renderscript" )
  __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
  if( NOT __availableToolchains AND NOT ANDROID_NDK_TOOLCHAINS_SUBPATH STREQUAL ANDROID_NDK_TOOLCHAINS_SUBPATH2 )
   __GLOB_NDK_TOOLCHAINS( __availableToolchains __availableToolchainsLst "${ANDROID_NDK_TOOLCHAINS_SUBPATH2}" )
   if( __availableToolchains )
    set( ANDROID_NDK_TOOLCHAINS_SUBPATH ${ANDROID_NDK_TOOLCHAINS_SUBPATH2} )
   endif()
  endif()
 endif()
 if( NOT __availableToolchains )
  message( FATAL_ERROR "Could not find any working toolchain in the NDK. Probably your Android NDK is broken." )
 endif()
endif()

# build list of available ABIs
set( ANDROID_SUPPORTED_ABIS "" )
set( __uniqToolchainArchNames ${__availableToolchainArchs} )
list( REMOVE_DUPLICATES __uniqToolchainArchNames )
list( SORT __uniqToolchainArchNames )
foreach( __arch ${__uniqToolchainArchNames} )
 list( APPEND ANDROID_SUPPORTED_ABIS ${ANDROID_SUPPORTED_ABIS_${__arch}} )
endforeach()
unset( __uniqToolchainArchNames )
if( NOT ANDROID_SUPPORTED_ABIS )
 message( FATAL_ERROR "No one of known Android ABIs is supported by this cmake toolchain." )
endif()

# choose target ABI
__INIT_VARIABLE( ANDROID_ABI VALUES ${ANDROID_SUPPORTED_ABIS} )
# verify that target ABI is supported
list( FIND ANDROID_SUPPORTED_ABIS "${ANDROID_ABI}" __androidAbiIdx )
if( __androidAbiIdx EQUAL -1 )
 string( REPLACE ";" "\", \"" PRINTABLE_ANDROID_SUPPORTED_ABIS  "${ANDROID_SUPPORTED_ABIS}" )
 message( FATAL_ERROR "Specified ANDROID_ABI = \"${ANDROID_ABI}\" is not supported by this cmake toolchain or your NDK/toolchain.
   Supported values are: \"${PRINTABLE_ANDROID_SUPPORTED_ABIS}\"
   " )
endif()
unset( __androidAbiIdx )

# set target ABI options
if( ANDROID_ABI STREQUAL "x86" )
 set( X86 true )
 set( ANDROID_NDK_ABI_NAME "x86" )
 set( ANDROID_ARCH_NAME "x86" )
 set( ANDROID_LLVM_TRIPLE "i686-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "i686" )
elseif( ANDROID_ABI STREQUAL "x86_64" )
 set( X86 true )
 set( X86_64 true )
 set( ANDROID_NDK_ABI_NAME "x86_64" )
 set( ANDROID_ARCH_NAME "x86_64" )
 set( CMAKE_SYSTEM_PROCESSOR "x86_64" )
 set( ANDROID_LLVM_TRIPLE "x86_64-none-linux-android" )
elseif( ANDROID_ABI STREQUAL "mips64" )
 set( MIPS64 true )
 set( ANDROID_NDK_ABI_NAME "mips64" )
 set( ANDROID_ARCH_NAME "mips64" )
 set( ANDROID_LLVM_TRIPLE "mips64el-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "mips64" )
elseif( ANDROID_ABI STREQUAL "mips" )
 set( MIPS true )
 set( ANDROID_NDK_ABI_NAME "mips" )
 set( ANDROID_ARCH_NAME "mips" )
 set( ANDROID_LLVM_TRIPLE "mipsel-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "mips" )
elseif( ANDROID_ABI STREQUAL "arm64-v8a" )
 set( ARM64_V8A true )
 set( ANDROID_NDK_ABI_NAME "arm64-v8a" )
 set( ANDROID_ARCH_NAME "arm64" )
 set( ANDROID_LLVM_TRIPLE "aarch64-none-linux-android" )
 set( CMAKE_SYSTEM_PROCESSOR "aarch64" )
 set( VFPV3 true )
 set( NEON true )
elseif( ANDROID_ABI STREQUAL "armeabi" )
 set( ARMEABI true )
 set( ANDROID_NDK_ABI_NAME "armeabi" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv5te-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv5te" )
elseif( ANDROID_ABI STREQUAL "armeabi-v6 with VFP" )
 set( ARMEABI_V6 true )
 set( ANDROID_NDK_ABI_NAME "armeabi" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv5te-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv6" )
 # need always fallback to older platform
 set( ARMEABI true )
elseif( ANDROID_ABI STREQUAL "armeabi-v7a")
 set( ARMEABI_V7A true )
 set( ANDROID_NDK_ABI_NAME "armeabi-v7a" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv7-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
elseif( ANDROID_ABI STREQUAL "armeabi-v7a with VFPV3" )
 set( ARMEABI_V7A true )
 set( ANDROID_NDK_ABI_NAME "armeabi-v7a" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv7-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
 set( VFPV3 true )
elseif( ANDROID_ABI STREQUAL "armeabi-v7a with NEON" )
 set( ARMEABI_V7A true )
 set( ANDROID_NDK_ABI_NAME "armeabi-v7a" )
 set( ANDROID_ARCH_NAME "arm" )
 set( ANDROID_LLVM_TRIPLE "armv7-none-linux-androideabi" )
 set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
 set( VFPV3 true )
 set( NEON true )
else()
 message( SEND_ERROR "Unknown ANDROID_ABI=\"${ANDROID_ABI}\" is specified." )
endif()

if( CMAKE_BINARY_DIR AND EXISTS "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeSystem.cmake" )
 # really dirty hack
 # it is not possible to change CMAKE_SYSTEM_PROCESSOR after the first run...
 file( APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeSystem.cmake" "SET(CMAKE_SYSTEM_PROCESSOR \"${CMAKE_SYSTEM_PROCESSOR}\")\n" )
endif()

if( ANDROID_ARCH_NAME STREQUAL "arm" AND NOT ARMEABI_V6 )
 __INIT_VARIABLE( ANDROID_FORCE_ARM_BUILD VALUES OFF )
 set( ANDROID_FORCE_ARM_BUILD ${ANDROID_FORCE_ARM_BUILD} CACHE BOOL "Use 32-bit ARM instructions instead of Thumb-1" FORCE )
 mark_as_advanced( ANDROID_FORCE_ARM_BUILD )
else()
 unset( ANDROID_FORCE_ARM_BUILD CACHE )
endif()

# choose toolchain
if( ANDROID_TOOLCHAIN_NAME )
 list( FIND __availableToolchains "${ANDROID_TOOLCHAIN_NAME}" __toolchainIdx )
 if( __toolchainIdx EQUAL -1 )
  list( SORT __availableToolchains )
  string( REPLACE ";" "\n  * " toolchains_list "${__availableToolchains}" )
  set( toolchains_list "  * ${toolchains_list}")
  message( FATAL_ERROR "Specified toolchain \"${ANDROID_TOOLCHAIN_NAME}\" is missing in your NDK or broken. Please verify that your NDK is working or select another compiler toolchain.
To configure the toolchain set CMake variable ANDROID_TOOLCHAIN_NAME to one of the following values:\n${toolchains_list}\n" )
 endif()
 list( GET __availableToolchainArchs ${__toolchainIdx} __toolchainArch )
 if( NOT __toolchainArch STREQUAL ANDROID_ARCH_NAME )
  message( SEND_ERROR "Selected toolchain \"${ANDROID_TOOLCHAIN_NAME}\" is not able to compile binaries for the \"${ANDROID_ARCH_NAME}\" platform." )
 endif()
else()
 set( __toolchainIdx -1 )
 set( __applicableToolchains "" )
 set( __toolchainMaxVersion "0.0.0" )
 list( LENGTH __availableToolchains __availableToolchainsCount )
 math( EXPR __availableToolchainsCount "${__availableToolchainsCount}-1" )
 foreach( __idx RANGE ${__availableToolchainsCount} )
  list( GET __availableToolchainArchs ${__idx} __toolchainArch )
  if( __toolchainArch STREQUAL ANDROID_ARCH_NAME )
   list( GET __availableToolchainCompilerVersions ${__idx} __toolchainVersion )
   string( REPLACE "x" "99" __toolchainVersion "${__toolchainVersion}")
   if( __toolchainVersion VERSION_GREATER __toolchainMaxVersion )
    set( __toolchainMaxVersion "${__toolchainVersion}" )
    set( __toolchainIdx ${__idx} )
   endif()
  endif()
 endforeach()
 unset( __availableToolchainsCount )
 unset( __toolchainMaxVersion )
 unset( __toolchainVersion )
endif()
unset( __toolchainArch )
if( __toolchainIdx EQUAL -1 )
 message( FATAL_ERROR "No one of available compiler toolchains is able to compile for ${ANDROID_ARCH_NAME} platform." )
endif()
list( GET __availableToolchains ${__toolchainIdx} ANDROID_TOOLCHAIN_NAME )
list( GET __availableToolchainMachines ${__toolchainIdx} ANDROID_TOOLCHAIN_MACHINE_NAME )
list( GET __availableToolchainCompilerVersions ${__toolchainIdx} ANDROID_COMPILER_VERSION )

unset( __toolchainIdx )
unset( __availableToolchains )
unset( __availableToolchainMachines )
unset( __availableToolchainArchs )
unset( __availableToolchainCompilerVersions )

# choose native API level
__INIT_VARIABLE( ANDROID_NATIVE_API_LEVEL ENV_ANDROID_NATIVE_API_LEVEL ANDROID_API_LEVEL ENV_ANDROID_API_LEVEL ANDROID_STANDALONE_TOOLCHAIN_API_LEVEL ANDROID_DEFAULT_NDK_API_LEVEL_${ANDROID_ARCH_NAME} ANDROID_DEFAULT_NDK_API_LEVEL )
string( REPLACE "android-" "" ANDROID_NATIVE_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}" )
string( STRIP "${ANDROID_NATIVE_API_LEVEL}" ANDROID_NATIVE_API_LEVEL )
# adjust API level
set( __real_api_level ${ANDROID_DEFAULT_NDK_API_LEVEL_${ANDROID_ARCH_NAME}} )
foreach( __level ${ANDROID_SUPPORTED_NATIVE_API_LEVELS} )
 if( (__level LESS ANDROID_NATIVE_API_LEVEL OR __level STREQUAL ANDROID_NATIVE_API_LEVEL) AND NOT __level LESS __real_api_level )
  set( __real_api_level ${__level} )
 endif()
endforeach()
if( __real_api_level AND NOT ANDROID_NATIVE_API_LEVEL STREQUAL __real_api_level )
 message( STATUS "Adjusting Android API level 'android-${ANDROID_NATIVE_API_LEVEL}' to 'android-${__real_api_level}'")
 set( ANDROID_NATIVE_API_LEVEL ${__real_api_level} )
endif()
unset(__real_api_level)
# validate
list( FIND ANDROID_SUPPORTED_NATIVE_API_LEVELS "${ANDROID_NATIVE_API_LEVEL}" __levelIdx )
if( __levelIdx EQUAL -1 )
 message( SEND_ERROR "Specified Android native API level 'android-${ANDROID_NATIVE_API_LEVEL}' is not supported by your NDK/toolchain." )
else()
 if( BUILD_WITH_ANDROID_NDK )
  __DETECT_NATIVE_API_LEVEL( __realApiLevel "${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}/usr/include/android/api-level.h" )
  if( NOT __realApiLevel EQUAL ANDROID_NATIVE_API_LEVEL AND NOT __realApiLevel GREATER 9000 )
   message( SEND_ERROR "Specified Android API level (${ANDROID_NATIVE_API_LEVEL}) does not match to the level found (${__realApiLevel}). Probably your copy of NDK is broken." )
  endif()
  unset( __realApiLevel )
 endif()
 set( ANDROID_NATIVE_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}" CACHE STRING "Android API level for native code" FORCE )
 set( CMAKE_ANDROID_API ${ANDROID_NATIVE_API_LEVEL} )
 if( CMAKE_VERSION VERSION_GREATER "2.8" )
  list( SORT ANDROID_SUPPORTED_NATIVE_API_LEVELS )
  set_property( CACHE ANDROID_NATIVE_API_LEVEL PROPERTY STRINGS ${ANDROID_SUPPORTED_NATIVE_API_LEVELS} )
 endif()
endif()
unset( __levelIdx )


# remember target ABI
set( ANDROID_ABI "${ANDROID_ABI}" CACHE STRING "The target ABI for Android. If arm, then armeabi-v7a is recommended for hardware floating point." FORCE )
if( CMAKE_VERSION VERSION_GREATER "2.8" )
 list( SORT ANDROID_SUPPORTED_ABIS_${ANDROID_ARCH_NAME} )
 set_property( CACHE ANDROID_ABI PROPERTY STRINGS ${ANDROID_SUPPORTED_ABIS_${ANDROID_ARCH_NAME}} )
endif()


# runtime choice (STL, rtti, exceptions)
if( NOT ANDROID_STL )
  set( ANDROID_STL gnustl_static )
endif()
set( ANDROID_STL "${ANDROID_STL}" CACHE STRING "C++ runtime" )
set( ANDROID_STL_FORCE_FEATURES ON CACHE BOOL "automatically configure rtti and exceptions support based on C++ runtime" )
mark_as_advanced( ANDROID_STL ANDROID_STL_FORCE_FEATURES )

if( BUILD_WITH_ANDROID_NDK )
 if( NOT "${ANDROID_STL}" MATCHES "^(none|system|system_re|gabi\\+\\+_static|gabi\\+\\+_shared|stlport_static|stlport_shared|gnustl_static|gnustl_shared)$")
  message( FATAL_ERROR "ANDROID_STL is set to invalid value \"${ANDROID_STL}\".
The possible values are:
  none           -> Do not configure the runtime.
  system         -> Use the default minimal system C++ runtime library.
  system_re      -> Same as system but with rtti and exceptions.
  gabi++_static  -> Use the GAbi++ runtime as a static library.
  gabi++_shared  -> Use the GAbi++ runtime as a shared library.
  stlport_static -> Use the STLport runtime as a static library.
  stlport_shared -> Use the STLport runtime as a shared library.
  gnustl_static  -> (default) Use the GNU STL as a static library.
  gnustl_shared  -> Use the GNU STL as a shared library.
" )
 endif()
elseif( BUILD_WITH_STANDALONE_TOOLCHAIN )
 if( NOT "${ANDROID_STL}" MATCHES "^(none|gnustl_static|gnustl_shared)$")
  message( FATAL_ERROR "ANDROID_STL is set to invalid value \"${ANDROID_STL}\".
The possible values are:
  none           -> Do not configure the runtime.
  gnustl_static  -> (default) Use the GNU STL as a static library.
  gnustl_shared  -> Use the GNU STL as a shared library.
" )
 endif()
endif()

unset( ANDROID_RTTI )
unset( ANDROID_EXCEPTIONS )
unset( ANDROID_STL_INCLUDE_DIRS )
unset( __libstl )
unset( __libsupcxx )

if( NOT _CMAKE_IN_TRY_COMPILE AND ANDROID_NDK_RELEASE STREQUAL "r7b" AND ARMEABI_V7A AND NOT VFPV3 AND ANDROID_STL MATCHES "gnustl" )
 message( WARNING  "The GNU STL armeabi-v7a binaries from NDK r7b can crash non-NEON devices. The files provided with NDK r7b were not configured properly, resulting in crashes on Tegra2-based devices and others when trying to use certain floating-point functions (e.g., cosf, sinf, expf).
You are strongly recommended to switch to another NDK release.
" )
endif()

if( NOT _CMAKE_IN_TRY_COMPILE AND X86 AND ANDROID_STL MATCHES "gnustl" AND ANDROID_NDK_RELEASE STREQUAL "r6" )
  message( WARNING  "The x86 system header file from NDK r6 has incorrect definition for ptrdiff_t. You are recommended to upgrade to a newer NDK release or manually patch the header:
See https://android.googlesource.com/platform/development.git f907f4f9d4e56ccc8093df6fee54454b8bcab6c2
  diff --git a/ndk/platforms/android-9/arch-x86/include/machine/_types.h b/ndk/platforms/android-9/arch-x86/include/machine/_types.h
  index 5e28c64..65892a1 100644
  --- a/ndk/platforms/android-9/arch-x86/include/machine/_types.h
  +++ b/ndk/platforms/android-9/arch-x86/include/machine/_types.h
  @@ -51,7 +51,11 @@ typedef long int       ssize_t;
   #endif
   #ifndef _PTRDIFF_T
   #define _PTRDIFF_T
  -typedef long           ptrdiff_t;
  +#  ifdef __ANDROID__
  +     typedef int            ptrdiff_t;
  +#  else
  +     typedef long           ptrdiff_t;
  +#  endif
   #endif
" )
endif()


# setup paths and STL for standalone toolchain
if( BUILD_WITH_STANDALONE_TOOLCHAIN )
 set( ANDROID_TOOLCHAIN_ROOT "${ANDROID_STANDALONE_TOOLCHAIN}" )
 set( ANDROID_CLANG_TOOLCHAIN_ROOT "${ANDROID_STANDALONE_TOOLCHAIN}" )
 set( ANDROID_SYSROOT "${ANDROID_STANDALONE_TOOLCHAIN}/sysroot" )

 if( NOT ANDROID_STL STREQUAL "none" )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_STANDALONE_TOOLCHAIN}/include/c++/${ANDROID_COMPILER_VERSION}" )
  if( NOT EXISTS "${ANDROID_STL_INCLUDE_DIRS}" )
   # old location ( pre r8c )
   set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/include/c++/${ANDROID_COMPILER_VERSION}" )
  endif()
  if( ARMEABI_V7A AND EXISTS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/${CMAKE_SYSTEM_PROCESSOR}/bits" )
   list( APPEND ANDROID_STL_INCLUDE_DIRS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/${CMAKE_SYSTEM_PROCESSOR}" )
  elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/thumb/bits" )
   list( APPEND ANDROID_STL_INCLUDE_DIRS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/thumb" )
  else()
   list( APPEND ANDROID_STL_INCLUDE_DIRS "${ANDROID_STL_INCLUDE_DIRS}/${ANDROID_TOOLCHAIN_MACHINE_NAME}" )
  endif()
  # always search static GNU STL to get the location of libsupc++.a
  if( ARMEABI_V7A AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb" )
  elseif( ARMEABI_V7A AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}" )
  elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb" )
  elseif( EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libstdc++.a" )
   set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib" )
  endif()
  if( __libstl )
   set( __libsupcxx "${__libstl}/libsupc++.a" )
   set( __libstl    "${__libstl}/libstdc++.a" )
  endif()
  if( NOT EXISTS "${__libsupcxx}" )
   message( FATAL_ERROR "The required libstdsupc++.a is missing in your standalone toolchain.
 Usually it happens because of bug in make-standalone-toolchain.sh script from NDK r7, r7b and r7c.
 You need to either upgrade to newer NDK or manually copy
     $ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/libs/${ANDROID_NDK_ABI_NAME}/libsupc++.a
 to
     ${__libsupcxx}
   " )
  endif()
  if( ANDROID_STL STREQUAL "gnustl_shared" )
   if( ARMEABI_V7A AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libgnustl_shared.so" )
    set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libgnustl_shared.so" )
   elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD AND EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libgnustl_shared.so" )
    set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libgnustl_shared.so" )
   elseif( EXISTS "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libgnustl_shared.so" )
    set( __libstl "${ANDROID_STANDALONE_TOOLCHAIN}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libgnustl_shared.so" )
   endif()
  endif()
 endif()
endif()

# clang
if( "${ANDROID_TOOLCHAIN_NAME}" STREQUAL "standalone-clang" )
 set( ANDROID_COMPILER_IS_CLANG 1 )
 execute_process( COMMAND "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/clang${TOOL_OS_SUFFIX}" --version OUTPUT_VARIABLE ANDROID_CLANG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE )
 string( REGEX MATCH "[0-9]+[.][0-9]+" ANDROID_CLANG_VERSION "${ANDROID_CLANG_VERSION}")
elseif( "${ANDROID_TOOLCHAIN_NAME}" MATCHES "-clang3[.][0-9]?$" )
 string( REGEX MATCH "3[.][0-9]$" ANDROID_CLANG_VERSION "${ANDROID_TOOLCHAIN_NAME}")
 string( REGEX REPLACE "-clang${ANDROID_CLANG_VERSION}$" "-${ANDROID_COMPILER_VERSION}" ANDROID_GCC_TOOLCHAIN_NAME "${ANDROID_TOOLCHAIN_NAME}" )
 if( NOT EXISTS "${ANDROID_NDK_TOOLCHAINS_PATH}/llvm-${ANDROID_CLANG_VERSION}${ANDROID_NDK_TOOLCHAINS_SUBPATH}/bin/clang${TOOL_OS_SUFFIX}" )
  message( FATAL_ERROR "Could not find the Clang compiler driver" )
 endif()
 set( ANDROID_COMPILER_IS_CLANG 1 )
 set( ANDROID_CLANG_TOOLCHAIN_ROOT "${ANDROID_NDK_TOOLCHAINS_PATH}/llvm-${ANDROID_CLANG_VERSION}${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
else()
 set( ANDROID_GCC_TOOLCHAIN_NAME "${ANDROID_TOOLCHAIN_NAME}" )
 unset( ANDROID_COMPILER_IS_CLANG CACHE )
endif()

string( REPLACE "." "" _clang_name "clang${ANDROID_CLANG_VERSION}" )
if( NOT EXISTS "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}${TOOL_OS_SUFFIX}" )
 set( _clang_name "clang" )
endif()


# setup paths and STL for NDK
if( BUILD_WITH_ANDROID_NDK )
 set( ANDROID_TOOLCHAIN_ROOT "${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}${ANDROID_NDK_TOOLCHAINS_SUBPATH}" )
 set( ANDROID_SYSROOT "${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}" )

 if( ANDROID_STL STREQUAL "none" )
  # do nothing
 elseif( ANDROID_STL STREQUAL "system" )
  set( ANDROID_RTTI             OFF )
  set( ANDROID_EXCEPTIONS       OFF )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/system/include" )
 elseif( ANDROID_STL STREQUAL "system_re" )
  set( ANDROID_RTTI             ON )
  set( ANDROID_EXCEPTIONS       ON )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/system/include" )
 elseif( ANDROID_STL MATCHES "gabi" )
  if( ANDROID_NDK_RELEASE_NUM LESS 7000 ) # before r7
   message( FATAL_ERROR "gabi++ is not awailable in your NDK. You have to upgrade to NDK r7 or newer to use gabi++.")
  endif()
  set( ANDROID_RTTI             ON )
  set( ANDROID_EXCEPTIONS       OFF )
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/gabi++/include" )
  set( __libstl                 "${ANDROID_NDK}/sources/cxx-stl/gabi++/libs/${ANDROID_NDK_ABI_NAME}/libgabi++_static.a" )
 elseif( ANDROID_STL MATCHES "stlport" )
  if( NOT ANDROID_NDK_RELEASE_NUM LESS 8004 ) # before r8d
   set( ANDROID_EXCEPTIONS       ON )
  else()
   set( ANDROID_EXCEPTIONS       OFF )
  endif()
  if( ANDROID_NDK_RELEASE_NUM LESS 7000 ) # before r7
   set( ANDROID_RTTI            OFF )
  else()
   set( ANDROID_RTTI            ON )
  endif()
  set( ANDROID_STL_INCLUDE_DIRS "${ANDROID_NDK}/sources/cxx-stl/stlport/stlport" )
  set( __libstl                 "${ANDROID_NDK}/sources/cxx-stl/stlport/libs/${ANDROID_NDK_ABI_NAME}/libstlport_static.a" )
 elseif( ANDROID_STL MATCHES "gnustl" )
  set( ANDROID_EXCEPTIONS       ON )
  set( ANDROID_RTTI             ON )
  if( EXISTS "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_COMPILER_VERSION}" )
   if( ARMEABI_V7A AND ANDROID_COMPILER_VERSION VERSION_EQUAL "4.7" AND ANDROID_NDK_RELEASE STREQUAL "r8d" )
    # gnustl binary for 4.7 compiler is buggy :(
    # TODO: look for right fix
    set( __libstl                "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.6" )
   else()
    set( __libstl                "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_COMPILER_VERSION}" )
   endif()
  else()
   set( __libstl                "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++" )
  endif()
  set( ANDROID_STL_INCLUDE_DIRS "${__libstl}/include" "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/include" "${__libstl}/include/backward" )
  if( EXISTS "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/libgnustl_static.a" )
   set( __libstl                "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/libgnustl_static.a" )
  else()
   set( __libstl                "${__libstl}/libs/${ANDROID_NDK_ABI_NAME}/libstdc++.a" )
  endif()
 else()
  message( FATAL_ERROR "Unknown runtime: ${ANDROID_STL}" )
 endif()
 # find libsupc++.a - rtti & exceptions
 if( ANDROID_STL STREQUAL "system_re" OR ANDROID_STL MATCHES "gnustl" )
  set( __libsupcxx "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_COMPILER_VERSION}/libs/${ANDROID_NDK_ABI_NAME}/libsupc++.a" ) # r8b or newer
  if( NOT EXISTS "${__libsupcxx}" )
   set( __libsupcxx "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/libs/${ANDROID_NDK_ABI_NAME}/libsupc++.a" ) # r7-r8
  endif()
  if( NOT EXISTS "${__libsupcxx}" ) # before r7
   if( ARMEABI_V7A )
    if( ANDROID_FORCE_ARM_BUILD )
     set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/libsupc++.a" )
    else()
     set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb/libsupc++.a" )
    endif()
   elseif( ARMEABI AND NOT ANDROID_FORCE_ARM_BUILD )
    set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/thumb/libsupc++.a" )
   else()
    set( __libsupcxx "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib/libsupc++.a" )
   endif()
  endif()
  if( NOT EXISTS "${__libsupcxx}")
   message( ERROR "Could not find libsupc++.a for a chosen platform. Either your NDK is not supported or is broken.")
  endif()
 endif()
endif()


# case of shared STL linkage
if( ANDROID_STL MATCHES "shared" AND DEFINED __libstl )
 string( REPLACE "_static.a" "_shared.so" __libstl "${__libstl}" )
 # TODO: check if .so file exists before the renaming
endif()


# ccache support
__INIT_VARIABLE( _ndk_ccache NDK_CCACHE ENV_NDK_CCACHE )
if( _ndk_ccache )
 if( DEFINED NDK_CCACHE AND NOT EXISTS NDK_CCACHE )
  unset( NDK_CCACHE CACHE )
 endif()
 find_program( NDK_CCACHE "${_ndk_ccache}" DOC "The path to ccache binary")
else()
 unset( NDK_CCACHE CACHE )
endif()
unset( _ndk_ccache )


# setup the cross-compiler
if( NOT CMAKE_C_COMPILER )
 if( NDK_CCACHE AND NOT ANDROID_SYSROOT MATCHES "[ ;\"]" )
  set( CMAKE_C_COMPILER   "${NDK_CCACHE}" CACHE PATH "ccache as C compiler" )
  set( CMAKE_CXX_COMPILER "${NDK_CCACHE}" CACHE PATH "ccache as C++ compiler" )
  if( ANDROID_COMPILER_IS_CLANG )
   set( CMAKE_C_COMPILER_ARG1   "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}${TOOL_OS_SUFFIX}"   CACHE PATH "C compiler")
   set( CMAKE_CXX_COMPILER_ARG1 "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}++${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler")
  else()
   set( CMAKE_C_COMPILER_ARG1   "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gcc${TOOL_OS_SUFFIX}" CACHE PATH "C compiler")
   set( CMAKE_CXX_COMPILER_ARG1 "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-g++${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler")
  endif()
 else()
  if( ANDROID_COMPILER_IS_CLANG )
   set( CMAKE_C_COMPILER   "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}${TOOL_OS_SUFFIX}"   CACHE PATH "C compiler")
   set( CMAKE_CXX_COMPILER "${ANDROID_CLANG_TOOLCHAIN_ROOT}/bin/${_clang_name}++${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler")
  else()
   set( CMAKE_C_COMPILER   "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gcc${TOOL_OS_SUFFIX}"    CACHE PATH "C compiler" )
   set( CMAKE_CXX_COMPILER "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-g++${TOOL_OS_SUFFIX}"    CACHE PATH "C++ compiler" )
  endif()
 endif()
 set( CMAKE_ASM_COMPILER "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "assembler" )
 set( CMAKE_STRIP        "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" )
 set( CMAKE_AR           "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" )
 set( CMAKE_LINKER       "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" )
 set( CMAKE_NM           "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" )
 set( CMAKE_OBJCOPY      "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" )
 set( CMAKE_OBJDUMP      "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" )
 set( CMAKE_RANLIB       "${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" )
endif()

set( _CMAKE_TOOLCHAIN_PREFIX "${ANDROID_TOOLCHAIN_MACHINE_NAME}-" )
if( CMAKE_VERSION VERSION_LESS 2.8.5 )
 set( CMAKE_ASM_COMPILER_ARG1 "-c" )
endif()
if( APPLE )
 find_program( CMAKE_INSTALL_NAME_TOOL NAMES install_name_tool )
 if( NOT CMAKE_INSTALL_NAME_TOOL )
  message( FATAL_ERROR "Could not find install_name_tool, please check your installation." )
 endif()
 mark_as_advanced( CMAKE_INSTALL_NAME_TOOL )
endif()

# Force set compilers because standard identification works badly for us
include( CMakeForceCompiler )
CMAKE_FORCE_C_COMPILER( "${CMAKE_C_COMPILER}" GNU )
if( ANDROID_COMPILER_IS_CLANG )
 set( CMAKE_C_COMPILER_ID Clang )
endif()
set( CMAKE_C_PLATFORM_ID Linux )
if( X86_64 OR MIPS64 OR ARM64_V8A )
 set( CMAKE_C_SIZEOF_DATA_PTR 8 )
else()
 set( CMAKE_C_SIZEOF_DATA_PTR 4 )
endif()
set( CMAKE_C_HAS_ISYSROOT 1 )
set( CMAKE_C_COMPILER_ABI ELF )
CMAKE_FORCE_CXX_COMPILER( "${CMAKE_CXX_COMPILER}" GNU )
if( ANDROID_COMPILER_IS_CLANG )
 set( CMAKE_CXX_COMPILER_ID Clang)
endif()
set( CMAKE_CXX_PLATFORM_ID Linux )
set( CMAKE_CXX_SIZEOF_DATA_PTR ${CMAKE_C_SIZEOF_DATA_PTR} )
set( CMAKE_CXX_HAS_ISYSROOT 1 )
set( CMAKE_CXX_COMPILER_ABI ELF )
set( CMAKE_CXX_SOURCE_FILE_EXTENSIONS cc cp cxx cpp CPP c++ C )
# force ASM compiler (required for CMake < 2.8.5)
set( CMAKE_ASM_COMPILER_ID_RUN TRUE )
set( CMAKE_ASM_COMPILER_ID GNU )
set( CMAKE_ASM_COMPILER_WORKS TRUE )
set( CMAKE_ASM_COMPILER_FORCED TRUE )
set( CMAKE_COMPILER_IS_GNUASM 1)
set( CMAKE_ASM_SOURCE_FILE_EXTENSIONS s S asm )

foreach( lang C CXX ASM )
 if( ANDROID_COMPILER_IS_CLANG )
  set( CMAKE_${lang}_COMPILER_VERSION ${ANDROID_CLANG_VERSION} )
 else()
  set( CMAKE_${lang}_COMPILER_VERSION ${ANDROID_COMPILER_VERSION} )
 endif()
endforeach()

# flags and definitions
remove_definitions( -DANDROID )
add_definitions( -DANDROID )

if( ANDROID_SYSROOT MATCHES "[ ;\"]" )
 if( CMAKE_HOST_WIN32 )
  # try to convert path to 8.3 form
  file( WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cvt83.cmd" "@echo %~s1" )
  execute_process( COMMAND "$ENV{ComSpec}" /c "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cvt83.cmd" "${ANDROID_SYSROOT}"
                   OUTPUT_VARIABLE __path OUTPUT_STRIP_TRAILING_WHITESPACE
                   RESULT_VARIABLE __result ERROR_QUIET )
  if( __result EQUAL 0 )
   file( TO_CMAKE_PATH "${__path}" ANDROID_SYSROOT )
   set( ANDROID_CXX_FLAGS "--sysroot=${ANDROID_SYSROOT}" )
  else()
   set( ANDROID_CXX_FLAGS "--sysroot=\"${ANDROID_SYSROOT}\"" )
  endif()
 else()
  set( ANDROID_CXX_FLAGS "'--sysroot=${ANDROID_SYSROOT}'" )
 endif()
 if( NOT _CMAKE_IN_TRY_COMPILE )
  # quotes can break try_compile and compiler identification
  message(WARNING "Path to your Android NDK (or toolchain) has non-alphanumeric symbols.\nThe build might be broken.\n")
 endif()
else()
 set( ANDROID_CXX_FLAGS "--sysroot=${ANDROID_SYSROOT}" )
endif()

# NDK flags
if (ARM64_V8A )
 set( ANDROID_CXX_FLAGS         "${ANDROID_CXX_FLAGS} -funwind-tables" )
 set( ANDROID_CXX_FLAGS_RELEASE "-fomit-frame-pointer -fstrict-aliasing" )
 set( ANDROID_CXX_FLAGS_DEBUG   "-fno-omit-frame-pointer -fno-strict-aliasing" )
 if( NOT ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS_RELEASE "${ANDROID_CXX_FLAGS_RELEASE} -funswitch-loops -finline-limit=300" )
 endif()
elseif( ARMEABI OR ARMEABI_V7A)
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funwind-tables" )
 if( NOT ANDROID_FORCE_ARM_BUILD AND NOT ARMEABI_V6 )
  set( ANDROID_CXX_FLAGS_RELEASE "-mthumb -fomit-frame-pointer -fno-strict-aliasing" )
  set( ANDROID_CXX_FLAGS_DEBUG   "-marm -fno-omit-frame-pointer -fno-strict-aliasing" )
  if( NOT ANDROID_COMPILER_IS_CLANG )
   set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -finline-limit=64" )
  endif()
 else()
  # always compile ARMEABI_V6 in arm mode; otherwise there is no difference from ARMEABI
  set( ANDROID_CXX_FLAGS_RELEASE "-marm -fomit-frame-pointer -fstrict-aliasing" )
  set( ANDROID_CXX_FLAGS_DEBUG   "-marm -fno-omit-frame-pointer -fno-strict-aliasing" )
  if( NOT ANDROID_COMPILER_IS_CLANG )
   set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funswitch-loops -finline-limit=300" )
  endif()
 endif()
elseif( X86 OR X86_64 )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funwind-tables" )
 if( NOT ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -funswitch-loops -finline-limit=300" )
 endif()
 set( ANDROID_CXX_FLAGS_RELEASE "-fomit-frame-pointer -fstrict-aliasing" )
 set( ANDROID_CXX_FLAGS_DEBUG   "-fno-omit-frame-pointer -fno-strict-aliasing" )
elseif( MIPS OR MIPS64 )
 set( ANDROID_CXX_FLAGS         "${ANDROID_CXX_FLAGS} -fno-strict-aliasing -finline-functions -funwind-tables -fmessage-length=0" )
 set( ANDROID_CXX_FLAGS_RELEASE "-fomit-frame-pointer" )
 set( ANDROID_CXX_FLAGS_DEBUG   "-fno-omit-frame-pointer" )
 if( NOT ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers" )
  set( ANDROID_CXX_FLAGS_RELEASE "${ANDROID_CXX_FLAGS_RELEASE} -funswitch-loops -finline-limit=300" )
 endif()
elseif()
 set( ANDROID_CXX_FLAGS_RELEASE "" )
 set( ANDROID_CXX_FLAGS_DEBUG   "" )
endif()

set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -fsigned-char" ) # good/necessary when porting desktop libraries

if( NOT X86 AND NOT ANDROID_COMPILER_IS_CLANG )
 set( ANDROID_CXX_FLAGS "-Wno-psabi ${ANDROID_CXX_FLAGS}" )
endif()

if( NOT ANDROID_COMPILER_VERSION VERSION_LESS "4.6" )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -no-canonical-prefixes" ) # see https://android-review.googlesource.com/#/c/47564/
endif()

# ABI-specific flags
if( ARMEABI_V7A )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -march=armv7-a -mfloat-abi=softfp" )
 if( NEON )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -mfpu=neon" )
 elseif( VFPV3 )
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -mfpu=vfpv3" )
 else()
  set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -mfpu=vfpv3-d16" )
 endif()
elseif( ARMEABI_V6 )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -march=armv6 -mfloat-abi=softfp -mfpu=vfp" ) # vfp == vfpv2
elseif( ARMEABI )
 set( ANDROID_CXX_FLAGS "${ANDROID_CXX_FLAGS} -march=armv5te -mtune=xscale -msoft-float" )
endif()

if( ANDROID_STL MATCHES "gnustl" AND (EXISTS "${__libstl}" OR EXISTS "${__libsupcxx}") )
 set( CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_CREATE_SHARED_MODULE  "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_LINK_EXECUTABLE       "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
else()
 set( CMAKE_CXX_CREATE_SHARED_LIBRARY "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_CREATE_SHARED_MODULE  "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
 set( CMAKE_CXX_LINK_EXECUTABLE       "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
endif()

# STL
if( EXISTS "${__libstl}" OR EXISTS "${__libsupcxx}" )
 if( EXISTS "${__libstl}" )
  set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} \"${__libstl}\"" )
  set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} \"${__libstl}\"" )
  set( CMAKE_CXX_LINK_EXECUTABLE       "${CMAKE_CXX_LINK_EXECUTABLE} \"${__libstl}\"" )
 endif()
 if( EXISTS "${__libsupcxx}" )
  set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} \"${__libsupcxx}\"" )
  set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} \"${__libsupcxx}\"" )
  set( CMAKE_CXX_LINK_EXECUTABLE       "${CMAKE_CXX_LINK_EXECUTABLE} \"${__libsupcxx}\"" )
  # C objects:
  set( CMAKE_C_CREATE_SHARED_LIBRARY "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_C_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_C_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
  set( CMAKE_C_CREATE_SHARED_MODULE  "<CMAKE_C_COMPILER> <CMAKE_SHARED_LIBRARY_C_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_C_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>" )
  set( CMAKE_C_LINK_EXECUTABLE       "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>" )
  set( CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY} \"${__libsupcxx}\"" )
  set( CMAKE_C_CREATE_SHARED_MODULE  "${CMAKE_C_CREATE_SHARED_MODULE} \"${__libsupcxx}\"" )
  set( CMAKE_C_LINK_EXECUTABLE       "${CMAKE_C_LINK_EXECUTABLE} \"${__libsupcxx}\"" )
 endif()
 if( ANDROID_STL MATCHES "gnustl" )
  if( NOT EXISTS "${ANDROID_LIBM_PATH}" )
   set( ANDROID_LIBM_PATH -lm )
  endif()
  set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} ${ANDROID_LIBM_PATH}" )
  set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} ${ANDROID_LIBM_PATH}" )
  set( CMAKE_CXX_LINK_EXECUTABLE       "${CMAKE_CXX_LINK_EXECUTABLE} ${ANDROID_LIBM_PATH}" )
 endif()
endif()

# variables controlling optional build flags
if( ANDROID_NDK_RELEASE_NUM LESS 7000 ) # before r7
 # libGLESv2.so in NDK's prior to r7 refers to missing external symbols.
 # So this flag option is required for all projects using OpenGL from native.
 __INIT_VARIABLE( ANDROID_SO_UNDEFINED                      VALUES ON )
else()
 __INIT_VARIABLE( ANDROID_SO_UNDEFINED                      VALUES OFF )
endif()
__INIT_VARIABLE( ANDROID_NO_UNDEFINED                       VALUES ON )
__INIT_VARIABLE( ANDROID_FUNCTION_LEVEL_LINKING             VALUES ON )
__INIT_VARIABLE( ANDROID_GOLD_LINKER                        VALUES ON )
__INIT_VARIABLE( ANDROID_NOEXECSTACK                        VALUES ON )
__INIT_VARIABLE( ANDROID_RELRO                              VALUES ON )

set( ANDROID_NO_UNDEFINED           ${ANDROID_NO_UNDEFINED}           CACHE BOOL "Show all undefined symbols as linker errors" )
set( ANDROID_SO_UNDEFINED           ${ANDROID_SO_UNDEFINED}           CACHE BOOL "Allows or disallows undefined symbols in shared libraries" )
set( ANDROID_FUNCTION_LEVEL_LINKING ${ANDROID_FUNCTION_LEVEL_LINKING} CACHE BOOL "Put each function in separate section and enable garbage collection of unused input sections at link time" )
set( ANDROID_GOLD_LINKER            ${ANDROID_GOLD_LINKER}            CACHE BOOL "Enables gold linker" )
set( ANDROID_NOEXECSTACK            ${ANDROID_NOEXECSTACK}            CACHE BOOL "Allows or disallows undefined symbols in shared libraries" )
set( ANDROID_RELRO                  ${ANDROID_RELRO}                  CACHE BOOL "Enables RELRO - a memory corruption mitigation technique" )
mark_as_advanced( ANDROID_NO_UNDEFINED ANDROID_SO_UNDEFINED ANDROID_FUNCTION_LEVEL_LINKING ANDROID_GOLD_LINKER ANDROID_NOEXECSTACK ANDROID_RELRO )

# linker flags
set( ANDROID_LINKER_FLAGS "" )

if( ARMEABI_V7A )
 # this is *required* to use the following linker flags that routes around
 # a CPU bug in some Cortex-A8 implementations:
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--fix-cortex-a8" )
endif()

if( ANDROID_NO_UNDEFINED )
 if( MIPS )
  # there is some sysroot-related problem in mips linker...
  if( NOT ANDROID_SYSROOT MATCHES "[ ;\"]" )
   set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--no-undefined -Wl,-rpath-link,${ANDROID_SYSROOT}/usr/lib" )
  endif()
 else()
  set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--no-undefined" )
 endif()
endif()

if( ANDROID_SO_UNDEFINED )
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,-allow-shlib-undefined" )
endif()

if( ANDROID_FUNCTION_LEVEL_LINKING )
 set( ANDROID_CXX_FLAGS    "${ANDROID_CXX_FLAGS} -fdata-sections -ffunction-sections" )
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,--gc-sections" )
endif()

if( ANDROID_COMPILER_VERSION VERSION_EQUAL "4.6" )
 if( ANDROID_GOLD_LINKER AND (CMAKE_HOST_UNIX OR ANDROID_NDK_RELEASE_NUM GREATER 8002) AND (ARMEABI OR ARMEABI_V7A OR X86) )
  set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -fuse-ld=gold" )
 elseif( ANDROID_NDK_RELEASE_NUM GREATER 8002 ) # after r8b
  set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -fuse-ld=bfd" )
 elseif( ANDROID_NDK_RELEASE STREQUAL "r8b" AND ARMEABI AND NOT _CMAKE_IN_TRY_COMPILE )
  message( WARNING "The default bfd linker from arm GCC 4.6 toolchain can fail with 'unresolvable R_ARM_THM_CALL relocation' error message. See https://code.google.com/p/android/issues/detail?id=35342
  On Linux and OS X host platform you can workaround this problem using gold linker (default).
  Rerun cmake with -DANDROID_GOLD_LINKER=ON option in case of problems.
" )
 endif()
endif() # version 4.6

if( ANDROID_NOEXECSTACK )
 if( ANDROID_COMPILER_IS_CLANG )
  set( ANDROID_CXX_FLAGS    "${ANDROID_CXX_FLAGS} -Xclang -mnoexecstack" )
 else()
  set( ANDROID_CXX_FLAGS    "${ANDROID_CXX_FLAGS} -Wa,--noexecstack" )
 endif()
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,-z,noexecstack" )
endif()

if( ANDROID_RELRO )
 set( ANDROID_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now" )
endif()

if( ANDROID_COMPILER_IS_CLANG )
 set( ANDROID_CXX_FLAGS "-target ${ANDROID_LLVM_TRIPLE} -Qunused-arguments ${ANDROID_CXX_FLAGS}" )
 if( BUILD_WITH_ANDROID_NDK )
  set( ANDROID_CXX_FLAGS "-gcc-toolchain ${ANDROID_TOOLCHAIN_ROOT} ${ANDROID_CXX_FLAGS}" )
 endif()
endif()

# cache flags
set( CMAKE_CXX_FLAGS           ""                        CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS             ""                        CACHE STRING "c flags" )
set( CMAKE_CXX_FLAGS_RELEASE   "-O3 -DNDEBUG"            CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE     "-O3 -DNDEBUG"            CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG       "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c Debug flags" )
set( CMAKE_SHARED_LINKER_FLAGS ""                        CACHE STRING "shared linker flags" )
set( CMAKE_MODULE_LINKER_FLAGS ""                        CACHE STRING "module linker flags" )
set( CMAKE_EXE_LINKER_FLAGS    "-Wl,-z,nocopyreloc"      CACHE STRING "executable linker flags" )

# put flags to cache (for debug purpose only)
set( ANDROID_CXX_FLAGS         "${ANDROID_CXX_FLAGS}"         CACHE INTERNAL "Android specific c/c++ flags" )
set( ANDROID_CXX_FLAGS_RELEASE "${ANDROID_CXX_FLAGS_RELEASE}" CACHE INTERNAL "Android specific c/c++ Release flags" )
set( ANDROID_CXX_FLAGS_DEBUG   "${ANDROID_CXX_FLAGS_DEBUG}"   CACHE INTERNAL "Android specific c/c++ Debug flags" )
set( ANDROID_LINKER_FLAGS      "${ANDROID_LINKER_FLAGS}"      CACHE INTERNAL "Android specific c/c++ linker flags" )

# finish flags
set( CMAKE_CXX_FLAGS           "${ANDROID_CXX_FLAGS} ${CMAKE_CXX_FLAGS}" )
set( CMAKE_C_FLAGS             "${ANDROID_CXX_FLAGS} ${CMAKE_C_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE   "${ANDROID_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}" )
set( CMAKE_C_FLAGS_RELEASE     "${ANDROID_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}" )
set( CMAKE_CXX_FLAGS_DEBUG     "${ANDROID_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_DEBUG}" )
set( CMAKE_C_FLAGS_DEBUG       "${ANDROID_CXX_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG}" )
set( CMAKE_SHARED_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}" )
set( CMAKE_MODULE_LINKER_FLAGS "${ANDROID_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS    "${ANDROID_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}" )

if( MIPS AND BUILD_WITH_ANDROID_NDK AND ANDROID_NDK_RELEASE STREQUAL "r8" )
 set( CMAKE_SHARED_LINKER_FLAGS "-Wl,-T,${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}/mipself.xsc ${CMAKE_SHARED_LINKER_FLAGS}" )
 set( CMAKE_MODULE_LINKER_FLAGS "-Wl,-T,${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}/mipself.xsc ${CMAKE_MODULE_LINKER_FLAGS}" )
 set( CMAKE_EXE_LINKER_FLAGS    "-Wl,-T,${ANDROID_NDK_TOOLCHAINS_PATH}/${ANDROID_GCC_TOOLCHAIN_NAME}/mipself.x ${CMAKE_EXE_LINKER_FLAGS}" )
endif()

# pie/pic
if( NOT (ANDROID_NATIVE_API_LEVEL LESS 16) AND (NOT DEFINED ANDROID_APP_PIE OR ANDROID_APP_PIE) AND (CMAKE_VERSION VERSION_GREATER 2.8.8) )
 set( CMAKE_POSITION_INDEPENDENT_CODE TRUE )
 set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
else()
 set( CMAKE_POSITION_INDEPENDENT_CODE FALSE )
 set( CMAKE_CXX_FLAGS "-fpic ${CMAKE_CXX_FLAGS}" )
 set( CMAKE_C_FLAGS   "-fpic ${CMAKE_C_FLAGS}" )
endif()

# configure rtti
if( DEFINED ANDROID_RTTI AND ANDROID_STL_FORCE_FEATURES )
 if( ANDROID_RTTI )
  set( CMAKE_CXX_FLAGS "-frtti ${CMAKE_CXX_FLAGS}" )
 else()
  set( CMAKE_CXX_FLAGS "-fno-rtti ${CMAKE_CXX_FLAGS}" )
 endif()
endif()

# configure exceptios
if( DEFINED ANDROID_EXCEPTIONS AND ANDROID_STL_FORCE_FEATURES )
 if( ANDROID_EXCEPTIONS )
  set( CMAKE_CXX_FLAGS "-fexceptions ${CMAKE_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "-fexceptions ${CMAKE_C_FLAGS}" )
 else()
  set( CMAKE_CXX_FLAGS "-fno-exceptions ${CMAKE_CXX_FLAGS}" )
  set( CMAKE_C_FLAGS "-fno-exceptions ${CMAKE_C_FLAGS}" )
 endif()
endif()

# global includes and link directories
include_directories( SYSTEM "${ANDROID_SYSROOT}/usr/include" ${ANDROID_STL_INCLUDE_DIRS} )
get_filename_component(__android_install_path "${CMAKE_INSTALL_PREFIX}/libs/${ANDROID_NDK_ABI_NAME}" ABSOLUTE) # avoid CMP0015 policy warning
link_directories( "${__android_install_path}" )

# detect if need link crtbegin_so.o explicitly
if( NOT DEFINED ANDROID_EXPLICIT_CRT_LINK )
 set( __cmd "${CMAKE_CXX_CREATE_SHARED_LIBRARY}" )
 string( REPLACE "<CMAKE_CXX_COMPILER>" "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}" __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_C_COMPILER>"   "${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ARG1}"   __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_SHARED_LIBRARY_CXX_FLAGS>" "${CMAKE_CXX_FLAGS}" __cmd "${__cmd}" )
 string( REPLACE "<LANGUAGE_COMPILE_FLAGS>" "" __cmd "${__cmd}" )
 string( REPLACE "<LINK_FLAGS>" "${CMAKE_SHARED_LINKER_FLAGS}" __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS>" "-shared" __cmd "${__cmd}" )
 string( REPLACE "<CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG>" "" __cmd "${__cmd}" )
 string( REPLACE "<TARGET_SONAME>" "" __cmd "${__cmd}" )
 string( REPLACE "<TARGET>" "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/toolchain_crtlink_test.so" __cmd "${__cmd}" )
 string( REPLACE "<OBJECTS>" "\"${ANDROID_SYSROOT}/usr/lib/crtbegin_so.o\"" __cmd "${__cmd}" )
 string( REPLACE "<LINK_LIBRARIES>" "" __cmd "${__cmd}" )
 separate_arguments( __cmd )
 foreach( __var ANDROID_NDK ANDROID_NDK_TOOLCHAINS_PATH ANDROID_STANDALONE_TOOLCHAIN )
  if( ${__var} )
   set( __tmp "${${__var}}" )
   separate_arguments( __tmp )
   string( REPLACE "${__tmp}" "${${__var}}" __cmd "${__cmd}")
  endif()
 endforeach()
 string( REPLACE "'" "" __cmd "${__cmd}" )
 string( REPLACE "\"" "" __cmd "${__cmd}" )
 execute_process( COMMAND ${__cmd} RESULT_VARIABLE __cmd_result OUTPUT_QUIET ERROR_QUIET )
 if( __cmd_result EQUAL 0 )
  set( ANDROID_EXPLICIT_CRT_LINK ON )
 else()
  set( ANDROID_EXPLICIT_CRT_LINK OFF )
 endif()
endif()

if( ANDROID_EXPLICIT_CRT_LINK )
 set( CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} \"${ANDROID_SYSROOT}/usr/lib/crtbegin_so.o\"" )
 set( CMAKE_CXX_CREATE_SHARED_MODULE  "${CMAKE_CXX_CREATE_SHARED_MODULE} \"${ANDROID_SYSROOT}/usr/lib/crtbegin_so.o\"" )
endif()

# setup output directories
set( CMAKE_INSTALL_PREFIX "${ANDROID_TOOLCHAIN_ROOT}/user" CACHE STRING "path for installing" )

if( DEFINED LIBRARY_OUTPUT_PATH_ROOT
      OR EXISTS "${CMAKE_SOURCE_DIR}/AndroidManifest.xml"
      OR (EXISTS "${CMAKE_SOURCE_DIR}/../AndroidManifest.xml" AND EXISTS "${CMAKE_SOURCE_DIR}/../jni/") )
  set( LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_SOURCE_DIR} CACHE PATH "Root for binaries output, set this to change where Android libs are installed to" )
  if( NOT _CMAKE_IN_TRY_COMPILE )
    if( EXISTS "${CMAKE_SOURCE_DIR}/jni/CMakeLists.txt" )
      set( EXECUTABLE_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH_ROOT}/bin/${ANDROID_NDK_ABI_NAME}" CACHE PATH "Output directory for applications" )
    else()
      set( EXECUTABLE_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH_ROOT}/bin" CACHE PATH "Output directory for applications" )
    endif()
    set( LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME}" CACHE PATH "Output directory for Android libs" )
  endif()
endif()

# copy shaed stl library to build directory
if( NOT _CMAKE_IN_TRY_COMPILE AND __libstl MATCHES "[.]so$" AND DEFINED LIBRARY_OUTPUT_PATH )
  get_filename_component( __libstlname "${__libstl}" NAME )
  execute_process( COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${__libstl}" "${LIBRARY_OUTPUT_PATH}/${__libstlname}" RESULT_VARIABLE __fileCopyProcess )
  if( NOT __fileCopyProcess EQUAL 0 OR NOT EXISTS "${LIBRARY_OUTPUT_PATH}/${__libstlname}")
    message( SEND_ERROR "Failed copying of ${__libstl} to the ${LIBRARY_OUTPUT_PATH}/${__libstlname}" )
  endif()
  unset( __fileCopyProcess )
  unset( __libstlname )
endif()


# set these global flags for cmake client scripts to change behavior
set( ANDROID True )
set( BUILD_ANDROID True )

# where is the target environment
set( CMAKE_FIND_ROOT_PATH "${ANDROID_TOOLCHAIN_ROOT}/bin" "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}" "${ANDROID_SYSROOT}" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share" )

# only search for libraries and includes in the ndk toolchain
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )


# macro to find packages on the host OS
macro( find_host_package )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
 if( CMAKE_HOST_WIN32 )
  SET( WIN32 1 )
  SET( UNIX )
 elseif( CMAKE_HOST_APPLE )
  SET( APPLE 1 )
  SET( UNIX )
 endif()
 find_package( ${ARGN} )
 SET( WIN32 )
 SET( APPLE )
 SET( UNIX 1 )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
endmacro()


# macro to find programs on the host OS
macro( find_host_program )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
 if( CMAKE_HOST_WIN32 )
  SET( WIN32 1 )
  SET( UNIX )
 elseif( CMAKE_HOST_APPLE )
  SET( APPLE 1 )
  SET( UNIX )
 endif()
 find_program( ${ARGN} )
 SET( WIN32 )
 SET( APPLE )
 SET( UNIX 1 )
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
endmacro()


# export toolchain settings for the try_compile() command
if( NOT _CMAKE_IN_TRY_COMPILE )
 set( __toolchain_config "")
 foreach( __var NDK_CCACHE  LIBRARY_OUTPUT_PATH_ROOT  ANDROID_FORBID_SYGWIN
                ANDROID_NDK_HOST_X64
                ANDROID_NDK
                ANDROID_NDK_LAYOUT
                ANDROID_STANDALONE_TOOLCHAIN
                ANDROID_TOOLCHAIN_NAME
                ANDROID_ABI
                ANDROID_NATIVE_API_LEVEL
                ANDROID_STL
                ANDROID_STL_FORCE_FEATURES
                ANDROID_FORCE_ARM_BUILD
                ANDROID_NO_UNDEFINED
                ANDROID_SO_UNDEFINED
                ANDROID_FUNCTION_LEVEL_LINKING
                ANDROID_GOLD_LINKER
                ANDROID_NOEXECSTACK
                ANDROID_RELRO
                ANDROID_LIBM_PATH
                ANDROID_EXPLICIT_CRT_LINK
                ANDROID_APP_PIE
                )
  if( DEFINED ${__var} )
   if( ${__var} MATCHES " ")
    set( __toolchain_config "${__toolchain_config}set( ${__var} \"${${__var}}\" CACHE INTERNAL \"\" )\n" )
   else()
    set( __toolchain_config "${__toolchain_config}set( ${__var} ${${__var}} CACHE INTERNAL \"\" )\n" )
   endif()
  endif()
 endforeach()
 file( WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/android.toolchain.config.cmake" "${__toolchain_config}" )
 unset( __toolchain_config )
endif()


# force cmake to produce / instead of \ in build commands for Ninja generator
if( CMAKE_GENERATOR MATCHES "Ninja" AND CMAKE_HOST_WIN32 )
 # it is a bad hack after all
 # CMake generates Ninja makefiles with UNIX paths only if it thinks that we are going to build with MinGW
 set( CMAKE_COMPILER_IS_MINGW TRUE ) # tell CMake that we are MinGW
 set( CMAKE_CROSSCOMPILING TRUE )    # stop recursion
 enable_language( C )
 enable_language( CXX )
 # unset( CMAKE_COMPILER_IS_MINGW ) # can't unset because CMake does not convert back-slashes in response files without it
 unset( MINGW )
endif()


# Variables controlling behavior or set by cmake toolchain:
#   ANDROID_ABI : "armeabi-v7a" (default), "armeabi", "armeabi-v7a with NEON", "armeabi-v7a with VFPV3", "armeabi-v6 with VFP", "x86", "mips", "arm64-v8a", "x86_64", "mips64"
#   ANDROID_NATIVE_API_LEVEL : 3,4,5,8,9,14,15,16,17,18,19,21 (depends on NDK version)
#   ANDROID_STL : gnustl_static/gnustl_shared/stlport_static/stlport_shared/gabi++_static/gabi++_shared/system_re/system/none
#   ANDROID_FORBID_SYGWIN : ON/OFF
#   ANDROID_NO_UNDEFINED : ON/OFF
#   ANDROID_SO_UNDEFINED : OFF/ON  (default depends on NDK version)
#   ANDROID_FUNCTION_LEVEL_LINKING : ON/OFF
#   ANDROID_GOLD_LINKER : ON/OFF
#   ANDROID_NOEXECSTACK : ON/OFF
#   ANDROID_RELRO : ON/OFF
#   ANDROID_FORCE_ARM_BUILD : ON/OFF
#   ANDROID_STL_FORCE_FEATURES : ON/OFF
#   ANDROID_LIBM_PATH : path to libm.so (set to something like $(TOP)/out/target/product/<product_name>/obj/lib/libm.so) to workaround unresolved `sincos`
# Can be set only at the first run:
#   ANDROID_NDK : path to your NDK install
#   NDK_CCACHE : path to your ccache executable
#   ANDROID_TOOLCHAIN_NAME : the NDK name of compiler toolchain
#   ANDROID_NDK_HOST_X64 : try to use x86_64 toolchain (default for x64 host systems)
#   ANDROID_NDK_LAYOUT : the inner NDK structure (RELEASE, LINARO, ANDROID)
#   LIBRARY_OUTPUT_PATH_ROOT : <any valid path>
#   ANDROID_STANDALONE_TOOLCHAIN
#
# Primary read-only variables:
#   ANDROID : always TRUE
#   ARMEABI : TRUE for arm v6 and older devices
#   ARMEABI_V6 : TRUE for arm v6
#   ARMEABI_V7A : TRUE for arm v7a
#   ARM64_V8A : TRUE for arm64-v8a
#   NEON : TRUE if NEON unit is enabled
#   VFPV3 : TRUE if VFP version 3 is enabled
#   X86 : TRUE if configured for x86
#   X86_64 : TRUE if configured for x86_64
#   MIPS : TRUE if configured for mips
#   MIPS64 : TRUE if configured for mips64
#   BUILD_WITH_ANDROID_NDK : TRUE if NDK is used
#   BUILD_WITH_STANDALONE_TOOLCHAIN : TRUE if standalone toolchain is used
#   ANDROID_NDK_HOST_SYSTEM_NAME : "windows", "linux-x86" or "darwin-x86" depending on host platform
#   ANDROID_NDK_ABI_NAME : "armeabi", "armeabi-v7a", "x86", "mips", "arm64-v8a", "x86_64", "mips64" depending on ANDROID_ABI
#   ANDROID_NDK_RELEASE : from r5 to r10d; set only for NDK
#   ANDROID_NDK_RELEASE_NUM : numeric ANDROID_NDK_RELEASE version (1000*major+minor)
#   ANDROID_ARCH_NAME : "arm", "x86", "mips", "arm64", "x86_64", "mips64" depending on ANDROID_ABI
#   ANDROID_SYSROOT : path to the compiler sysroot
#   TOOL_OS_SUFFIX : "" or ".exe" depending on host platform
#   ANDROID_COMPILER_IS_CLANG : TRUE if clang compiler is used
#
# Secondary (less stable) read-only variables:
#   ANDROID_COMPILER_VERSION : GCC version used (not Clang version)
#   ANDROID_CLANG_VERSION : version of clang compiler if clang is used
#   ANDROID_CXX_FLAGS : C/C++ compiler flags required by Android platform
#   ANDROID_SUPPORTED_ABIS : list of currently allowed values for ANDROID_ABI
#   ANDROID_TOOLCHAIN_MACHINE_NAME : "arm-linux-androideabi", "arm-eabi" or "i686-android-linux"
#   ANDROID_TOOLCHAIN_ROOT : path to the top level of toolchain (standalone or placed inside NDK)
#   ANDROID_CLANG_TOOLCHAIN_ROOT : path to clang tools
#   ANDROID_SUPPORTED_NATIVE_API_LEVELS : list of native API levels found inside NDK
#   ANDROID_STL_INCLUDE_DIRS : stl include paths
#   ANDROID_RTTI : if rtti is enabled by the runtime
#   ANDROID_EXCEPTIONS : if exceptions are enabled by the runtime
#   ANDROID_GCC_TOOLCHAIN_NAME : read-only, differs from ANDROID_TOOLCHAIN_NAME only if clang is used
#
# Defaults:
#   ANDROID_DEFAULT_NDK_API_LEVEL
#   ANDROID_DEFAULT_NDK_API_LEVEL_${ARCH}
#   ANDROID_NDK_SEARCH_PATHS
#   ANDROID_SUPPORTED_ABIS_${ARCH}
#   ANDROID_SUPPORTED_NDK_VERSIONS
    steps: {
 "description": "This file is to be read by androiddeployqt",
 "qt": "@QT_ANDROID_QT_ROOT@",
 "sdk": "@QT_ANDROID_SDK_ROOT@",
 "ndk": "@QT_ANDROID_NDK_ROOT@",
 "sdkBuildToolsRevision": "@QT_ANDROID_SDK_BUILDTOOLS_REVISION@",
 "toolchain-prefix": "@ANDROID_TOOLCHAIN_MACHINE_NAME@",
 "tool-prefix": "@ANDROID_TOOLCHAIN_MACHINE_NAME@",
 "toolchain-version": "@ANDROID_COMPILER_VERSION@",
 "ndk-host": "@ANDROID_NDK_HOST_SYSTEM_NAME@",
 "target-architecture": "@ANDROID_ABI@",
 "application-binary": "@QT_ANDROID_APP_PATH@",
 "android-package": "@QT_ANDROID_APP_PACKAGE_NAME@",
 "android-app-name": "@QT_ANDROID_APP_NAME@",
 @QT_ANDROID_APP_EXTRA_LIBS@
 "android-package-source-directory": "@QT_ANDROID_APP_PACKAGE_SOURCE_ROOT@"
}
      - name: #include "winver.h"

IDI_ICON1               ICON    DISCARDABLE     "@ICON_FILE@"

VS_VERSION_INFO VERSIONINFO
 FILEVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 PRODUCTVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 FILEFLAGS 0x0L
 FILEFLAGSMASK 0x3fL
 FILEOS 0x00040004L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "000004b0"
        BEGIN
            VALUE "CompanyName", "@COMPANY@"
            VALUE "FileDescription", "@PROJECT_NAME@"
            VALUE "FileVersion", "@PROJECT_VERSION@"
            VALUE "LegalCopyright", "@COPYRIGHT@"
            VALUE "InternalName", "@PROJECT_NAME@"
            VALUE "OriginalFilename", "@PROJECT_NAME@.exe"
            VALUE "ProductName", "@PROJECT_NAME@"
            VALUE "ProductVersion", "@PROJECT_VERSION@"
        END
    END
END

        id: deployment
        uses: actions/deploy-pages@v4