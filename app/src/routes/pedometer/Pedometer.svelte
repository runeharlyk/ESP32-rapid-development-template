<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import Spinner from '$lib/components/Spinner.svelte';
	import MdiDelete from '~icons/mdi/delete';
	import Hamster from '$lib/components/Hamster.svelte';
	import Stat from '$lib/components/daisy/Stat.svelte';
	import ResponsiveStats from '$lib/components/daisy/ResponsiveStats.svelte';
	import { formatTime, formatTimeRange, utcToHHMM } from '$lib/utilities/string-utilities';
	import type { Session, Sessions } from '$lib/types/models';
	import RunningChart from './runningChart.svelte';
	import RunningChartAccumulation from './runningChartAccumulation.svelte';

	let isLoading = true;

	let allTimeTopPace: number;
	let allTimeAveragePace: number;
	let totalDistance: number;
	let totalTimeInWheel: number;

	let statifiedSessions;

	let currentSession: {
		distance: number;
		topPace: number;
		end: number;
		start: number;
		timeRunning: number;
		averagePace: number;
		sessionMin: number;
	};

	let speedMetersPerSecond = 0;

	let isRunning = false;
	let stopTimer: string | number | NodeJS.Timeout | undefined;

	const diameterOfHamsterWheel = 0.19;
	const numberOfMagnets = 1;
	const circumference = (Math.PI * diameterOfHamsterWheel) / numberOfMagnets;

	let sessions: Sessions;

	type PacePackage = { time_elapsed: number };

	onMount(async () => {
		await getSessions();

		socket.on('step', (newData: PacePackage) => {
			if (!isRunning) {
				getSessions();
			}
			isRunning = true;

			speedMetersPerSecond = circumference / newData.time_elapsed;
			sessions[sessions.length - 1].times.push(newData.time_elapsed);
			sessions[sessions.length - 1].steps += 1;
			calculateStats();
			clearTimeout(stopTimer);
			stopTimer = setTimeout(() => {
				isRunning = false;
				speedMetersPerSecond = 0;
			}, 2000);
		});

		calculateStats();
		isLoading = false;
	});

	const getSessions = async () => {
		const response = await fetch('/api/v1/steps');
		const json = await response.json();
		sessions = json.sessions;
	};

	const calculateStats = () => {
		if (!sessions) return;
		statifiedSessions = sessions.map((session) => statify(session));

		currentSession = statifiedSessions[statifiedSessions.length - 1];

		totalDistance = sum(statifiedSessions.map((sess) => sess.distance));

		allTimeTopPace = Math.max(...statifiedSessions.map((sess) => sess.topPace), 0);

		allTimeAveragePace =
			sum(statifiedSessions.map((sess) => sess.averagePace)) / sessions.length || 0;

		totalTimeInWheel = sum(
			sessions
				.map((sess) => {
					if (sess.end < 1700000000 || sess.start < 1700000000) return 0;
					return sess.end - sess.start - (sess.times[0] || 0);
				})
				.filter(Boolean)
		);
	};

	const statify = (session: Session) => {
		const distance = session.steps * circumference;
		const speeds = session.times.slice(1).map((time) => (time === 0 ? 0 : circumference / time));
		const timeRunning = sum(speeds) || 0;
		const topPace = Math.max(...speeds);
		const averagePace = speeds.length ? timeRunning / speeds.length : 0;
		const sessionMin = (session.end || new Date().getTime() / 1000) - session.start;
		return {
			end: session.end,
			start: session.start,
			distance,
			timeRunning,
			topPace,
			averagePace,
			sessionMin
		};
	};

	const sum = (arr: number[]) => arr.reduce((total, current) => (total += current), 0);

	onDestroy(() => socket.off('step'));

	let rotationAngle = 0;
	let lastTime = 0;
	const speedControl = 0.5;

	const rotateDiv = (time: number) => {
		if (lastTime) {
			const deltaTime = time - lastTime;
			rotationAngle -= speedMetersPerSecond * deltaTime * speedControl;
		}
		lastTime = time;
		requestAnimationFrame(rotateDiv);
	};

	const reset = () => socket.sendEvent('reset_pedometer', '');

	const f = (val: number) => Math.max(val, 0).toFixed(1);
</script>

<SettingsCard collapsible={false}>
	<button class="btn absolute top-2 right-2" on:click={reset}>
		<MdiDelete class="w-8 h-8" />
	</button>

	<div class="grid place-items-center">
		<Hamster {speedMetersPerSecond} {rotationAngle} />
	</div>

	{#if isLoading}
		<Spinner />
	{:else}
		{#if currentSession}
			<h2 class="text-2xl p-4 text-center">{isRunning ? 'Current' : 'Last'} session</h2>
			<ResponsiveStats>
				<Stat>
					<span slot="title">Distance</span>
					{f(currentSession.distance)} m
					<span slot="desc">
						{formatTimeRange(currentSession.start, currentSession.end)}
						({formatTime(currentSession.sessionMin)})
					</span>
				</Stat>

				<Stat>
					<span slot="title">Top pace</span>
					{f(currentSession.topPace)} m/s
					<span slot="desc">↗︎</span>
				</Stat>

				<Stat>
					<span slot="title">Pace</span>
					{f(speedMetersPerSecond)} m/s
					<!-- <span slot="desc">↗︎</span> -->
				</Stat>
			</ResponsiveStats>
		{/if}

		{#if sessions}
			<h2 class="text-2xl p-4 text-center">All time stats</h2>
			<ResponsiveStats>
				<Stat>
					<span slot="title">Total Distance</span>
					{f(totalDistance)} m
					<span slot="desc">
						{formatTimeRange(sessions[0].start, currentSession.end)}
						({formatTime(totalTimeInWheel)})
					</span>
				</Stat>

				<Stat>
					<span slot="title">Top Speed</span>
					{f(allTimeTopPace)} m/s
					<span slot="desc">↗︎</span>
				</Stat>

				<Stat>
					<span slot="title">Average pace</span>
					{f(allTimeAveragePace)} m/s
					<span slot="desc">↗︎</span>
				</Stat>
			</ResponsiveStats>

			<RunningChart {sessions} {circumference} />
			<RunningChartAccumulation {sessions} {circumference} />
		{/if}
	{/if}
</SettingsCard>
