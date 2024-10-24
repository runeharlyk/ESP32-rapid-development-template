<script lang="ts">
	import type { LayoutData } from './$types';
	import { onDestroy, onMount } from 'svelte';
	import { telemetry } from '$lib/stores/telemetry';
	import { analytics } from '$lib/stores/analytics';
	import { socket } from '$lib/stores/socket';
	import { page } from '$app/stores';
	import { Modals, closeModal } from 'svelte-modals';
	import Toast from '$lib/components/toasts/Toast.svelte';
	import { notifications } from '$lib/components/toasts/notifications';
	import { fade } from 'svelte/transition';
	import '../app.css';
	import Menu from './menu.svelte';
	import Statusbar from './statusbar.svelte';
	import type { Analytics } from '$lib/types/models';
	import type { RSSI } from '$lib/types/models';
	import type { DownloadOTA } from '$lib/types/models';

	export let data: LayoutData;

	onMount(() => {
		socket.init(`ws://${window.location.host}/api/v1/ws/events`);

		addEventListeners();
	});

	onDestroy(() => {
		removeEventListeners();
	});

	const addEventListeners = () => {
		socket.on('open', handleOpen);
		socket.on('close', handleClose);
		socket.on('error', handleError);
		socket.on('rssi', handleNetworkStatus);
		socket.on('otastatus', handleOAT);
		if ($page.data.features.analytics) socket.on('analytics', handleAnalytics);
	};

	const removeEventListeners = () => {
		socket.off('analytics', handleAnalytics);
		socket.off('open', handleOpen);
		socket.off('close', handleClose);
		socket.off('rssi', handleNetworkStatus);
		socket.off('otastatus', handleOAT);
	};

	const handleOpen = () => {
		notifications.success('Connection to device established', 5000);
	};

	const handleClose = () => {
		notifications.error('Connection to device lost', 5000);
		telemetry.setRSSI({ rssi: 0, ssid: '' });
	};

	const handleError = (data: any) => console.error(data);

	const handleAnalytics = (data: Analytics) => analytics.addData(data);

	const handleNetworkStatus = (data: RSSI) => telemetry.setRSSI(data);

	const handleOAT = (data: DownloadOTA) => {
		notifications.send(`${data.toFixed(1)}`, 'progress', 4000, 'Update');
		console.log(data);

		// telemetry.setDownloadOTA(data);
	};

	let menuOpen = false;
</script>

<svelte:head>
	<title>{$page.data.title}</title>
</svelte:head>

<div class="drawer lg:drawer-open">
	<input id="main-menu" type="checkbox" class="drawer-toggle" bind:checked={menuOpen} />
	<div class="drawer-content flex flex-col">
		<!-- Status bar content here -->
		<Statusbar />

		<!-- Main page content here -->
		<slot />
	</div>
	<!-- Side Navigation -->
	<div class="drawer-side z-30 shadow-lg">
		<label for="main-menu" class="drawer-overlay" />
		<Menu
			on:menuClicked={() => {
				menuOpen = false;
			}}
		/>
	</div>
</div>

<Modals>
	<!-- svelte-ignore a11y-click-events-have-key-events -->
	<div
		slot="backdrop"
		class="fixed inset-0 z-40 max-h-full max-w-full bg-black/20 backdrop-blur"
		transition:fade
		on:click={closeModal}
	/>
</Modals>

<Toast />
