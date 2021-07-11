import { useState, useEffect } from "react";

import Serial from "../Serial";

export default function UsbConnect(props: any) {
  const [deviceFound, setDeviceFound] = useState(false);
  const [statusMessage, setStatusMessage] = useState("Device disconnected");

  useEffect(() => {
    async function getDevice() {
      let devices = await navigator.usb.getDevices();
      if (devices.length > 0) {
        let serial = new Serial(devices[0]);
        try {
          await serial.connect();
          await serial.reset();
          setStatusMessage("Device connected");
          setDeviceFound(true);
          props.connectCallback(serial);
        } catch (error) {
          setStatusMessage(error.toString());
        }
      } else {
        setDeviceFound(false);
      }
    }
    getDevice();
  }, []);

  async function connect() {
    if (!deviceFound) {
      const filters = [{ vendorId: 0x2341 }]; // Arduino vendor id
      let device = await navigator.usb.requestDevice({ filters: filters });
      setDeviceFound(true);
      let serial = new Serial(device);
      try {
        await serial.connect();
        setStatusMessage("Device connected");
        setDeviceFound(true);
        props.connectCallback(serial);
      } catch (error) {
        setStatusMessage(error.toString());
      }
    } else {
      props.disconnectCallback();
      setStatusMessage("Device disconnected");
      setDeviceFound(false);
    }
  }

  return (
    <div>
      <button onClick={connect}>
        {deviceFound ? "Disconnect" : "Connect"}
      </button>
      <p>Status: {statusMessage}</p>
    </div>
  );
}
