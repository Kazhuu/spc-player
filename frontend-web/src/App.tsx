import { useState } from "react";

import FileHandler from "components/FileHandler";
import SpcList from "components/SpcList";
import UsbConnect from "components/UsbConnect";

import SpcReader from "SpcReader";
import Serial from "Serial";
import SpcWriter from "SpcWriter";

export default function App() {
  const [spcReaderList, setSpcReaderList] = useState({});
  const [serial, setSerial] = useState<Serial | null>(null);

  async function fileCallback(file: File) {
    let buffer = await file.arrayBuffer();
    let spcReader = new SpcReader(file.name, buffer);
    setSpcReaderList((state) => ({ [spcReader.name()]: spcReader, ...state }));
  }

  async function serialConnect(newSerial: Serial) {
    console.log("connect");
    setSerial(newSerial);
  }

  async function serialDisconnect() {
    console.log("disconnect");
    setSerial(null);
  }

  async function reset() {
    if (serial) {
      let writer = new SpcWriter(serial);
      await writer.reset();
    }
  }

  async function play(spcReader: SpcReader): Promise<void> {
    if (serial) {
      let writer = new SpcWriter(serial);
      await writer.reset();
      await writer.writeCpuRegisters(
        spcReader.programCounter,
        spcReader.a,
        spcReader.x,
        spcReader.y,
        spcReader.stackPointer,
        spcReader.programStatusWord
      );
      console.log("play");
    }
  }

  return (
    <div>
      <div>
        <UsbConnect
          connectCallback={serialConnect}
          disconnectCallback={serialDisconnect}
        />
      </div>
      <button onClick={reset}>Reset</button>
      <div>
        <FileHandler fileCallback={fileCallback} />
      </div>
      <div>
        <SpcList spcReaderList={spcReaderList} playCallback={play} />
      </div>
    </div>
  );
}
