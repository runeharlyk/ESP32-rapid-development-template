<script lang="ts">
	import type { Size } from './types';

	export let size: 'none' | Size = 'md';
	export let type: 'spinner' | 'dots' | 'ring' | 'ball' | 'bars' | 'infinity' = 'spinner';
</script>

<span
	class="loading loading-xs {$$restProps.class}"
	class:loading-spinner={type === 'spinner'}
	class:loading-dots={type === 'dots'}
	class:loading-ring={type === 'ring'}
	class:loading-ball={type === 'ball'}
	class:loading-infinity={type === 'infinity'}
	class:loading-xs={size === 'xs'}
	class:loading-sm={size === 'sm'}
	class:loading-md={size === 'md'}
	class:loading-lg={size === 'lg'}
></span>
