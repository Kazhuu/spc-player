import { useState, useEffect } from "react";

import FileHandler from "./components/FileHandler";
import SpcList from "./components/SpcList";
import UsbConnect from "./components/UsbConnect";

import SpcReader from "./SpcReader";
import Serial from "./Serial";

function App() {
  const [spcReaderList, setSpcReaderList] = useState({});
  const [serial, setSerial] = useState(null);

  async function fileCallback(file) {
    let buffer = await file.arrayBuffer();
    let spcReader = new SpcReader(file.name, buffer);
    setSpcReaderList((state) => ({ [spcReader.name()]: spcReader, ...state }));
  }

  async function usbConnect(device) {
    setSerial(new Serial(device));
  }

  async function usbDisconnect() {
    if (serial) {
      await serial.disconnect();
      setSerial(null);
    }
  }

  useEffect(() => {
    if (serial) {
      serial
        .connect()
        .then(() => {
          console.log("connect");
        })
        .catch((error) => {
          console.log(error);
        });
    }
  }, [serial]);

  return (
    <div>
      <div>
        <UsbConnect
          connectCallback={usbConnect}
          disconnectCallback={usbDisconnect}
        />
      </div>
      <div>
        <FileHandler fileCallback={fileCallback} />
      </div>
      <div>
        <SpcList spcReaderList={spcReaderList} />
      </div>
    </div>
  );
}

export default App;
