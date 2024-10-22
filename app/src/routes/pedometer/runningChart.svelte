<script lang="ts">
	import { daisyColor } from '$lib/DaisyUiHelper';
	import type { Sessions } from '$lib/types/models';
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

		const intervalCount = 24 * 3; // 72 intervals of 20 minutes each
		const labels: string[] = [];
		const data: number[] = new Array(intervalCount).fill(0);

		filteredSessions.forEach((session) => {
			const sessionStart = Math.max(session.start, twentyFourHoursAgo);
			const sessionEnd = Math.min(session.end, now);
			const startInterval = Math.floor((sessionStart - twentyFourHoursAgo) / (20 * 60));
			const endInterval = Math.floor((sessionEnd - twentyFourHoursAgo) / (20 * 60));

			for (let interval = startInterval; interval <= endInterval; interval++) {
				if (interval >= 0 && interval < intervalCount) {
					data[interval] += session.steps / (endInterval - startInterval + 1);
				}
			}
		});

		for (let i = 0; i < intervalCount; i++) {
			const intervalTime = new Date(twentyFourHoursAgo * 1000 + i * 20 * 60 * 1000);
			const hours = intervalTime.getHours().toString().padStart(2, '0');
			const minutes = intervalTime.getMinutes().toString().padStart(2, '0');
			labels.push(`${hours}:${minutes}`);
		}

		return [labels, data];
	}

	onMount(() => {
		const [labels, data] = processData(sessions);

		chart = new Chart(chartElement, {
			type: 'line',
			data: {
				labels,
				datasets: [
					{
						label: 'meters [m]',
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
						title: {
							display: true,
							text: 'Time'
						},
						grid: {
							color: daisyColor('--bc', 10)
						},
						ticks: {
							color: daisyColor('--bc'),
							maxRotation: 90,
							minRotation: 90,
							autoSkip: true,
							maxTicksLimit: 24 // Show every hour label
						}
						// type: 'timeseries' // Time-based x-axis
					},
					y: {
						// type: 'linear',
						beginAtZero: true,
						title: {
							display: true,
							text: 'Meters',
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
