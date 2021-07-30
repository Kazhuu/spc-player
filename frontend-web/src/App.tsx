import "index.css";
import "App.css";

import { useState, useEffect } from "react";
import Container from "@material-ui/core/Container";
import Button from "@material-ui/core/Button";
import ButtonGroup from "@material-ui/core/ButtonGroup";

import FileHandler from "components/FileHandler";
import SpcList from "components/SpcList";
import UsbConnect from "components/UsbConnect";

import SpcReader from "SpcReader";
import Serial from "Serial";
import SpcClient from "SpcClient";

export default function App() {
  const [spcReaderList, setSpcReaderList] = useState({});
  const [spcClient, setSpcClient] = useState<SpcClient | null>(null);
  const [notificationText, setNotificationText] = useState("");
  const [playingSongName, setPlayingSongName] = useState<string>("");
  const [showProgress, setShowProgress] = useState(false);
  const [progress, setProgress] = useState(0);
  const [nextSong, setNextSong] = useState<SpcReader | null>(null);

  useEffect(() => {
    if (nextSong) {
      play(nextSong);
      setNextSong(null);
    }
  }, [nextSong]);

  async function fileCallback(file: File[]) {
    let newSpcReaderList: { [key: string]: SpcReader } = {};
    await Promise.all(
      file.map(async (file) => {
        let buffer = await file.arrayBuffer();
        const spcReader = new SpcReader(file.name, buffer);
        if (!spcReaderList.hasOwnProperty(spcReader.name())) {
          newSpcReaderList[spcReader.name()] = spcReader;
        }
      })
    );
    setSpcReaderList((state) => ({ ...newSpcReaderList, ...state }));
  }

  async function serialConnect(newSerial: Serial) {
    setSpcClient(new SpcClient(newSerial));
  }

  function reset() {
    if (spcClient) {
      setNextSong(null);
      spcClient
        .reset()
        .then(() => setNotificationText("SPC reset was successful"))
        .catch((error) => setNotificationText(error.message));
    }
  }

  function findNextSong(previousSong: SpcReader) {
    const previousSongName = previousSong.name();
    const songs = Object.entries(spcReaderList);
    let index = songs.findIndex(([name, _]) => name === previousSongName);
    if (index === -1 || index + 1 >= songs.length) {
      index = -1;
    }
    setNextSong(songs[index + 1][1] as SpcReader);
  }

  async function play(spcReader: SpcReader): Promise<void> {
    if (spcClient) {
      try {
        await spcClient.reset();
        setPlayingSongName(spcReader.name());
        setShowProgress(true);
        setProgress(0);
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
        await spcClient.writeRestOfTheRam(spcReader.restOfTheRam, (progress) =>
          setProgress(progress)
        );
        await spcClient.start();
        setNotificationText(`Successfully playing: ${spcReader.title()}`);
        setTimeout(() => findNextSong(spcReader), spcReader.songLengthInMs());
      } catch (error) {
        setNotificationText(error.message);
      }
      setShowProgress(false);
    }
  }

  function readPorts() {
    if (spcClient) {
      spcClient
        .readPorts()
        .then((portValues) => {
          setNotificationText(
            "Reading SPC ports was successful. Check console for the data."
          );
          console.log(portValues);
        })
        .catch((error) => {
          setNotificationText(error.message);
        });
    }
  }

  function readBootcode() {
    if (spcClient) {
      spcClient
        .readBootCode()
        .then((result) => {
          let bytes: any = [];
          result.forEach((byte) => {
            bytes.push(byte.toString(16));
          });
          console.log(bytes);
          setNotificationText(
            "Reading bootcode from Arduino was successful. Check console for the data"
          );
        })
        .catch((error) => {
          setNotificationText(error.message);
        });
    }
  }

  return (
    <div className="App">
      <Container>
        {!spcClient ? (
          <div className="App-connect">
            <UsbConnect connectCallback={serialConnect} />
          </div>
        ) : (
          <div>
            <div className="App-connect">
              <ButtonGroup
                color="primary"
                aria-label="outlined primary button group"
              >
                <Button onClick={reset} disableElevation>
                  Reset APU
                </Button>
                <Button onClick={readPorts} disableElevation>
                  Read Ports
                </Button>
                <Button onClick={readBootcode} disableElevation>
                  Read Bootcode
                </Button>
              </ButtonGroup>
              <label className="App-label">{notificationText}</label>
            </div>
            <FileHandler fileCallback={fileCallback} />
            <div>
              <SpcList
                spcReaderList={spcReaderList}
                playCallback={play}
                playingSongName={playingSongName}
                showProgress={showProgress}
                progress={progress}
              />
            </div>
          </div>
        )}
      </Container>
    </div>
  );
}
