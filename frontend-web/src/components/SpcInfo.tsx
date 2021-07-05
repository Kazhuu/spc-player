import SpcReader from "../SpcReader";

function SpcInfo({ spcReader }: { spcReader: SpcReader }) {
  return (
    <div>
      <input value={spcReader.name()} readOnly={true} />
    </div>
  );
}

export default SpcInfo;
