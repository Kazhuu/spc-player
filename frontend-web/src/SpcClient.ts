import Serial from "Serial";

const RAM_PACKET_SIZE = 1015;

export default class SpcClient {
  private serial: Serial;
  private encoder: TextEncoder;
  private decoder: TextDecoder;

  public constructor(serial: Serial) {
    this.serial = serial;
    this.encoder = new TextEncoder();
    this.decoder = new TextDecoder();
  }

  public async writeCpuRegisters(
    programCounter: Uint8Array,
    a: number,
    x: number,
    y: number,
    stackPointer: number,
    programStatusWord: number
  ) {
    if (programCounter.length !== 2) {
      return Promise.reject("Parameter programCounter length must be 2");
    }
    await this.serial.write(this.encode("C"));
    await this.serial.write(programCounter);
    await this.serial.write(this.numberToByte(a));
    await this.serial.write(this.numberToByte(x));
    await this.serial.write(this.numberToByte(y));
    await this.serial.write(this.numberToByte(stackPointer));
    await this.serial.write(this.numberToByte(programStatusWord));
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) !== "1") {
      return Promise.reject("Writing CPU registers failed");
    }
  }

  public async writeDspRegisters(dspRegisters: Uint8Array) {
    if (dspRegisters.length !== 128) {
      return Promise.reject("DSP requires 128 register values");
    }
    await this.serial.write(this.encode("D"));
    await this.serial.write(dspRegisters);
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) !== "1") {
      return Promise.reject("Writing DSP registers failed");
    }
  }

  public async writeFirstPageRam(firstPageRam: Uint8Array) {
    if (firstPageRam.length !== 256) {
      return Promise.reject("First page of the SPC ram requires 256 bytes");
    }
    await this.serial.write(this.encode("0"));
    await this.serial.write(firstPageRam);
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) !== "1") {
      return Promise.reject("Writing first page ram failed");
    }
  }

  public async writeSecondPageRam(secondPageRam: Uint8Array) {
    if (secondPageRam.length !== 256) {
      return Promise.reject("Second page of the SPC ram requires 256 bytes");
    }
    await this.serial.write(this.encode("1"));
    await this.serial.write(secondPageRam);
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) !== "1") {
      return Promise.reject("Writing second page ram failed");
    }
  }

  public async writeRestOfTheRam(restOfTheRam: Uint8Array) {
    if (restOfTheRam.length !== 64960) {
      return Promise.reject("Rest of the SPC ram requires 64960 bytes");
    }
    let packetCount = restOfTheRam.length / RAM_PACKET_SIZE;
    await this.serial.write(this.encode("2"));
    for (let packetIndex = 0; packetIndex < packetCount; ++packetIndex) {
      let byteIndexStart = packetIndex * RAM_PACKET_SIZE;
      let byteIndexEnd = byteIndexStart + RAM_PACKET_SIZE;
      await this.serial.write(restOfTheRam.slice(byteIndexStart, byteIndexEnd));
      let readResult = await this.serial.read(1);
      if (readResult.data && this.decode(readResult.data.buffer) !== "1") {
        return Promise.reject("Ram packet " + packetIndex + " transfer failed");
      }
    }
  }

  public async readBootCode(): Promise<Uint8Array> {
    await this.serial.write(this.encode("B"));
    let result = await this.serial.read(1);
    if (result.status !== "ok") {
      return Promise.reject("Reading bootcode size failed");
    }
    let bootCodeSize = this.byteToNumber(result.data!.buffer);
    let bootcode = await this.serial.read(bootCodeSize);
    if (result.status !== "ok") {
      return Promise.reject("Reading bootcode failed");
    }
    return new Uint8Array(bootcode.data!.buffer);
  }

  public async readPorts(): Promise<Uint8Array> {
    await this.serial.write(this.encode("Q"));
    let result = await this.serial.read(4);
    if (result.status !== "ok") {
      return Promise.reject("Reading ports failed");
    }
    return new Uint8Array(result.data!.buffer);
  }

  public async start() {
    await this.serial.write(this.encode("S"));
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) !== "1") {
      return Promise.reject("Staring SPC execution failed");
    }
  }

  public async reset() {
    await this.serial.write(this.encode("R"));
    let result = await this.serial.read(1);
    if (result.data && this.decode(result.data.buffer) !== "1") {
      return Promise.reject("SPC reset timed out");
    }
  }

  private encode(data: string) {
    return this.encoder.encode(data);
  }

  private decode(data: ArrayBuffer) {
    return this.decoder.decode(data);
  }

  private numberToByte(number: number) {
    let arr = new ArrayBuffer(1);
    let view = new DataView(arr);
    view.setUint8(0, number);
    return arr;
  }

  private byteToNumber(byte: ArrayBuffer): number {
    let view = new DataView(byte);
    return view.getUint8(0);
  }
}
