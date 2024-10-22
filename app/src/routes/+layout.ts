import type { LayoutLoad } from './$types';

export const prerender = false;
export const ssr = false;

export const load = (async ({ fetch }) => {
	const result = await fetch('/api/v1/features');
	const item = await result.json();
	return {
		features: item,
		title: 'ESP32 SvelteKit',
		github: 'runeharlyk/ESP32-rapid-development-template',
		copyright: '2024 runeharlyk',
		appName: 'ESP SvelteKit'
	};
}) satisfies LayoutLoad;
