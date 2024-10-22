import { sveltekit } from '@sveltejs/kit/vite';
import type { UserConfig } from 'vite';
import Icons from 'unplugin-icons/vite';
import viteLittleFS from './vite-plugin-littlefs';

const config: UserConfig = {
	plugins: [
		sveltekit(),
		Icons({
			compiler: 'svelte'
		}),
		// Shorten file names for LittleFS 32 char limit
		viteLittleFS()
	],
	server: {
		proxy: {
			'/api': {
				target: 'http://esp-sveltekit.local/',
				changeOrigin: true,
				ws: true
			},
			'/ws': {
				target: 'http://esp-sveltekit.local/',
				changeOrigin: true,
				ws: true
			}
		}
	}
};

export default config;
