<script lang="ts">
  import msgpack from 'msgpack-lite'
  const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e'
  const CHARACTERISTIC_TX_UUID = '6e400003-b5a3-f393-e0a9-e50e24dcca9e'
  const CHARACTERISTIC_RX_UUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'

  let log = ''
  let device, txChar, rxChar
  let msgCount = 0
  let lastMsgTime = Date.now()

  //   const log = (msg) => {
  //     document.getElementById("log").textContent += msg + "\n";
  //     document.getElementById("log").scrollTop = 1e9;
  //   };

  const updateMsgCounter = () => {
    const now = Date.now()
    const elapsed = (now - lastMsgTime) / 1000
    const msgPerSec = (msgCount / elapsed).toFixed(1)
    // document.getElementById(
    //   "msgCounter"
    // ).textContent = `Messages per second: ${msgPerSec}`;
    msgCount = 0
    lastMsgTime = now
  }

  setInterval(updateMsgCounter, 1000)

  const connect = async () => {
    device = await navigator.bluetooth.requestDevice({
      acceptAllDevices: true,
      optionalServices: [SERVICE_UUID]
    })
    const server = await device.gatt.connect()
    const service = await server.getPrimaryService(SERVICE_UUID)
    txChar = await service.getCharacteristic(CHARACTERISTIC_TX_UUID)
    rxChar = await service.getCharacteristic(CHARACTERISTIC_RX_UUID)
    await txChar.startNotifications()
    txChar.addEventListener('characteristicvaluechanged', e => {
      const data = msgpack.decode(new Uint8Array(e.target.value.buffer))
      //   log(
      //     "RX: " +
      //       JSON.stringify(data) +
      //       " (" +
      //       e.target.value.byteLength +
      //       " bytes)"
      //   );
      msgCount++

      if (data.type === 'TEMP') {
        // document.getElementById(
        //   "tempDisplay"
        // ).textContent = `Temperature: ${data.value.toFixed(2)}`;
      }
    })
    // document.getElementById("disconnect").disabled = false;
    // document.getElementById("send").disabled = false;
    // document.getElementById("connect").disabled = true;
    // log("Connected");
  }

  const disconnect = async () => {
    if (device && device.gatt.connected) {
      await device.gatt.disconnect()
      log('Disconnected')
    }
    // document.getElementById("disconnect").disabled = true;
    // document.getElementById("send").disabled = true;
    // document.getElementById("connect").disabled = false;
    // document.getElementById("tempDisplay").textContent = "Temperature: --";
    msgCount = 0
    lastMsgTime = Date.now()
  }

  const send = async () => {
    const input = document.getElementById('command')
    const value = input.value
    if (value) {
      const cmd = {
        type: 'CMD',
        lx: parseFloat(value)
      }
      const data = msgpack.encode(cmd)
      await rxChar.writeValue(data)
      //   log(
      //     "TX: " + JSON.stringify(cmd) + " (" + data.byteLength + " bytes)"
      //   );
      //   input.value = "";
    }
  }
</script>

<div class="p-4">
  <h1 class="text-xl">Welcome to SvelteKit</h1>
  <p>
    Visit <a href="https://svelte.dev/docs/kit">svelte.dev/docs/kit</a> to read the documentation
  </p>

  <button class="btn btn-primary">Connect</button>

  <pre>
    {log}
  </pre>
</div>
