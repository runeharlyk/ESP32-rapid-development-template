<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import { throttle } from '$lib/utilities/timing-utilities';

	type LightState = { brightness: number };

	let lightState: LightState = { brightness: 128 };

	const event = 'light';

	onMount(() => socket.on(event, (newState: LightState) => (lightState = newState)));
	onDestroy(() => socket.off(event));

	const emitChanges = () => socket.sendEvent(event, lightState);

	const throttledEmit = throttle(emitChanges, 250);
</script>

<SettingsCard collapsible={false}>
	<div class="grid place-items-center"></div>

	<h2 class="text-2xl p-4 text-center">Light demo</h2>
	<input class="range" type="range" bind:value={lightState.brightness} on:input={throttledEmit} />
</SettingsCard>
