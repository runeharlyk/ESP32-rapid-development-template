<script lang="ts">
	import { daisyColor } from '$lib/DaisyUiHelper';
	import type { Sessions, Session } from '$lib/types/models';
	import { Chart, registerables } from 'chart.js';
	import { onMount } from 'svelte';
	import { cubicOut } from 'svelte/easing';
	import { slide } from 'svelte/transition';

	Chart.register(...registerables);

	let chartElement: HTMLCanvasElement;
	let chart: Chart;

	export let sessions: Sessions;
	export let circumference: number;

	function processData(sessions: Sessions): [string[], number[]] {
		const now = Math.floor(Date.now() / 1000);
		const twentyFourHoursAgo = now - 24 * 60 * 60;
		const filteredSessions = sessions.filter((session) => session.end > twentyFourHoursAgo);

		// Sort sessions by start time
		filteredSessions.sort((a, b) => a.start - b.start);

		const data: { time: number; steps: number }[] = [];
		let cumulativeSteps = 0;

		// Create an entry for every minute
		for (let time = twentyFourHoursAgo; time <= now; time += 60) {
			data.push({ time, steps: cumulativeSteps });

			// Check if this minute is within any session
			const activeSession = filteredSessions.find(
				(session) => session.start <= time && session.end > time
			);

			if (activeSession) {
				// Distribute steps evenly across the session duration
				const sessionDuration = activeSession.end - activeSession.start;
				const stepsPerMinute = activeSession.steps / (sessionDuration / 60);
				cumulativeSteps += stepsPerMinute * circumference;
			}
		}

		// Ensure the last entry has the final cumulative step count
		data[data.length - 1].steps = cumulativeSteps;

		const labels = data.map((point) => {
			const date = new Date(point.time * 1000);
			return date.toLocaleTimeString(['da'], { hour: '2-digit', minute: '2-digit' });
		});

		const stepData = data.map((point) => Math.round(point.steps));

		return [labels, stepData];
	}

	onMount(() => {
		const [labels, data] = processData(sessions);

		chart = new Chart(chartElement, {
			type: 'line',
			data: {
				labels,
				datasets: [
					{
						label: 'Cumulative distance [m]',
						borderColor: daisyColor('--p'),
						backgroundColor: daisyColor('--p', 50),
						borderWidth: 2,
						data,
						yAxisID: 'y'
					}
				]
			},
			options: {
				maintainAspectRatio: false,
				responsive: true,
				plugins: {
					legend: {
						display: true
					},
					tooltip: {
						mode: 'index',
						intersect: false
					}
				},
				elements: {
					point: {
						radius: 1
					}
				},
				scales: {
					x: {
						grid: {
							color: daisyColor('--bc', 10)
						},
						ticks: {
							color: daisyColor('--bc'),
							maxRotation: 90,
							minRotation: 90,
							autoSkip: true,
							maxTicksLimit: 24
						}
						// type: 'timeseries' // Time-based x-axis
					},
					y: {
						// type: 'linear',
						beginAtZero: true,
						title: {
							display: true,
							text: 'Total distance',
							color: daisyColor('--bc'),
							font: {
								size: 16,
								weight: 'bold'
							}
						},
						position: 'left',
						// suggestedMin: 20,
						// suggestedMax: 100,
						grid: { color: daisyColor('--bc', 10) },
						ticks: {
							color: daisyColor('--bc')
						},
						border: { color: daisyColor('--bc', 10) }
					}
				}
			}
		});
	});
</script>

<div class="w-full overflow-x-auto">
	<div
		class="flex w-full flex-col space-y-1 h-52"
		transition:slide|local={{ duration: 300, easing: cubicOut }}
	>
		<canvas bind:this={chartElement} />
	</div>
</div>
