import { useRef, SyntheticEvent } from "react";

function FileHandler({
  fileCallback,
}: {
  fileCallback: { (arg0: File): void };
}) {
  const fileInput = useRef<HTMLInputElement>(null);

  const handleSubmit = (event: SyntheticEvent) => {
    event.preventDefault();
    if (
      fileInput !== null &&
      fileInput.current !== null &&
      fileInput.current.files !== null
    ) {
      let file = fileInput.current.files[0];
      fileCallback(file);
    }
  };

  return (
    <form onSubmit={handleSubmit}>
      <label>
        <input type="file" ref={fileInput} />
      </label>
      <button type="submit">Submit</button>
    </form>
  );
}

export default FileHandler;
