<script lang="ts">
  import { ble } from '$lib/ble-adapter'
  import { EventBus } from '$lib/event-bus'
  let log = ''
  let value = 0

  type Temp = {
    value: number
  }

  EventBus.subscribe<Temp>('temp', data => {
    log += JSON.stringify(data)
  })

  const publish = () => {
    ble.send([2, 1, { value: 110 }])
  }

  const subscribe = () => {
    ble.send([0, 1])
  }

  const unsubscribe = () => {
    ble.send([1, 1])
  }
</script>

<div class="p-4">
  <h1 class="text-xl">Connect to device</h1>
  <div>
    {#if $ble}
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

  <pre>
      {log}
    </pre>
</div>
