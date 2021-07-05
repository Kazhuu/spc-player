import SpcReader from "../SpcReader";

export default function SpcInfo({ spcReader }: { spcReader: SpcReader }) {
  let length = spcReader.metadata!.length || 0;
  let game = spcReader.metadata!.game || "";
  let artist = spcReader.metadata!.artist || "";
  let comments = spcReader.metadata!.comments || "";
  return (
    <tr>
      <td>{spcReader.name()}</td>
      <td>{game}</td>
      <td>{artist}</td>
      <td>{comments}</td>
      <td>{length}</td>
    </tr>
  );
}
