<script lang="ts">
  import { ble } from '$lib/transport/ble-adapter'
  import { MessageTopic, type Temp } from '$lib/interfaces/transport.interface'
  import { dataBroker } from '$lib/transport/databroker'

  let value = $state(0)
  let bleConnected = ble.connected
  let log: string[] = $state([])
  let subscriptionId = ''

  dataBroker.addTransport(ble)

  const handleTemp = (data: Temp) => {
    console.log('temp', data)
    log.push('rx: ' + JSON.stringify(data))
  }

  const publish = () => {
    const data = { value: 110 }
    log.push('Publish: ' + JSON.stringify(data))
    dataBroker.emit(MessageTopic.TEMP, data, subscriptionId)
  }

  const subscribe = () => {
    log.push('Subscribe (temp)')
    subscriptionId = dataBroker.on<Temp>(MessageTopic.TEMP, handleTemp)
  }

  const unsubscribe = () => {
    log.push('Unsubscribe (temp)')
    dataBroker.off(subscriptionId)
    subscriptionId = ''
  }
</script>

<div class="flex h-dvh flex-col p-4">
  <h1 class="text-xl">Connect to device</h1>
  <div>
    {#if $bleConnected}
      Connected
    {:else}
      Disconnected
    {/if}
  </div>

  <div class="my-2 flex gap-4">
    <button class="btn btn-primary" onclick={() => ble.connect()}>Connect</button>
    <button class="btn btn-primary" onclick={() => ble.disconnect()}>Disconnect</button>
  </div>

  <hr />

  <div class="my-2 flex gap-4">
    <button class="btn btn-primary" onclick={() => subscribe()}>Subscribe</button>
    <button class="btn btn-primary" onclick={() => unsubscribe()}>Unsubscribe</button>
    <button class="btn btn-primary" onclick={() => publish()}>Publish</button>
  </div>

  <hr />

  <div class="my-2 flex gap-4">
    <input class="input" type="number" bind:value />
    <button class="btn btn-primary" onclick={() => ble.send([2, 1, { value }])}>Send</button>
  </div>

  <textarea class="flex-1 rounded-md bg-gray-100 p-2 text-xs text-gray-500"
    >{log.join('\n')}</textarea>
</div>
