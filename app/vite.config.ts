import { sveltekit } from '@sveltejs/kit/vite';
import type { UserConfig } from 'vite';
import Icons from 'unplugin-icons/vite';
import viteLittleFS from './vite-plugin-littlefs';

const config: UserConfig = {
	plugins: [sveltekit(), Icons({ compiler: 'svelte' }), viteLittleFS()],
	server: {
		proxy: {
			'/api': {
				target: 'http://192.168.0.15',
				changeOrigin: true,
				ws: true
			}
		}
	}
};

export default config;
