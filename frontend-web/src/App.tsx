import { useState } from "react";

import FileHandler from "components/FileHandler";
import SpcList from "components/SpcList";
import UsbConnect from "components/UsbConnect";

import SpcReader from "SpcReader";
import Serial from "Serial";
import SpcClient from "SpcClient";

export default function App() {
  const [spcReaderList, setSpcReaderList] = useState({});
  const [spcClient, setSpcClient] = useState<SpcClient | null>(null);

  async function fileCallback(file: File) {
    let buffer = await file.arrayBuffer();
    let spcReader = new SpcReader(file.name, buffer);
    setSpcReaderList((state) => ({ [spcReader.name()]: spcReader, ...state }));
  }

  async function serialConnect(newSerial: Serial) {
    console.log("connect");
    setSpcClient(new SpcClient(newSerial));
  }

  async function serialDisconnect() {
    console.log("disconnect");
    setSpcClient(null);
  }

  async function reset() {
    if (spcClient) {
      await spcClient.reset();
      console.log("reset");
    }
  }

  async function play(spcReader: SpcReader): Promise<void> {
    if (spcClient) {
      await spcClient.reset();
      await spcClient.writeCpuRegisters(
        spcReader.programCounter,
        spcReader.a,
        spcReader.x,
        spcReader.y,
        spcReader.stackPointer,
        spcReader.programStatusWord
      );
      await spcClient.writeDspRegisters(spcReader.dspRegisters);
      await spcClient.writeFirstPageRam(spcReader.firstPageRam);
      await spcClient.writeSecondPageRam(spcReader.secondPageRam);
      await spcClient.writeRestOfTheRam(spcReader.restOfTheRam);
      await spcClient.start();
      console.log("play");
    }
  }

  async function readPorts() {
    if (spcClient) {
      let result = await spcClient.readPorts();
      console.log(result);
    }
  }

  async function readBootcode() {
    if (spcClient) {
      let result = await spcClient.readBootCode();
      console.log(result);
      let bytes: any = [];
      result.forEach((byte) => {
        bytes.push(byte.toString(16));
      });
      console.log(bytes);
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
      <button onClick={readPorts}>Read Ports</button>
      <button onClick={readBootcode}>Read Bootcode</button>
      <div>
        <FileHandler fileCallback={fileCallback} />
      </div>
      <div>
        <SpcList spcReaderList={spcReaderList} playCallback={play} />
      </div>
    </div>
  );
}
