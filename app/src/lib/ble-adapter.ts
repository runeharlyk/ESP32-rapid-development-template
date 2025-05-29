import msgpack from 'msgpack-lite'
import { writable } from 'svelte/store'
import { EventBus } from './event-bus'

const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e'
const CHARACTERISTIC_TX_UUID = '6e400003-b5a3-f393-e0a9-e50e24dcca9e'
const CHARACTERISTIC_RX_UUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'

function createBLEAdapter() {
  const { subscribe, set } = writable(false)
  let device, server, service, tx, rx

  const connect = async () => {
    device = await navigator.bluetooth.requestDevice({
      acceptAllDevices: true,
      optionalServices: [SERVICE_UUID]
    })
    server = await device.gatt.connect()
    service = await server.getPrimaryService(SERVICE_UUID)
    tx = await service.getCharacteristic(CHARACTERISTIC_TX_UUID)
    rx = await service.getCharacteristic(CHARACTERISTIC_RX_UUID)
    await tx.startNotifications()

    tx.addEventListener('characteristicvaluechanged', e => {
      const data = msgpack.decode(new Uint8Array(e.target.value.buffer))
    })
  }

  const disconnect = async () => {
    if (device && device.gatt.connected) {
      await device.gatt.disconnect()
    }
  }

  const send = async (msg: object) => {
    const payload = msgpack.encode(msg)
    await rx.writeValue(payload)
  }
}

export const ble = createBLEAdapter()
