import { useRef } from "react";

function FileHandler(props) {
  const fileInput = useRef(null);

  const handleSubmit = (event) => {
    event.preventDefault();
    let file = fileInput.current.files[0];
    props.fileCallback(file);
  };

  return (
    <form onSubmit={handleSubmit}>
      <label>
        <input type="file" ref={fileInput} />
      </label>
      <br />
      <button type="submit">Submit</button>
    </form>
  );
}

export default FileHandler;
