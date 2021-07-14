import { useState } from "react";

import SpcReader from "SpcReader";

export default function SpcInfo({
  spcReader,
  playCallback,
}: {
  spcReader: SpcReader;
  playCallback: { (arg0: SpcReader): Promise<void> };
}) {
  const [disabledPlay, setDisabledPlay] = useState(false);

  let length = spcReader.metadata!.length || 0;
  let game = spcReader.metadata!.game || "";
  let artist = spcReader.metadata!.artist || "";
  let comments = spcReader.metadata!.comments || "";

  async function play() {
    //setDisabledPlay(true);
    await playCallback(spcReader);
    //setDisabledPlay(false);
  }
  return (
    <tr>
      <td>{spcReader.name()}</td>
      <td>{game}</td>
      <td>{artist}</td>
      <td>{comments}</td>
      <td>{length}</td>
      <td>
        <button onClick={play} disabled={disabledPlay}>
          Play
        </button>
      </td>
    </tr>
  );
}
