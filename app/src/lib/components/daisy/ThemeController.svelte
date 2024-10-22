<script lang="ts">
	import Swap from './Swap.svelte';

	export let theme: string;
	export let variant: 'toggle' | 'swap';
</script>

{#if typeof theme === 'string' && variant === 'toggle'}
	<input type="checkbox" value={theme} class="toggle theme-controller" />
{:else if typeof theme === 'string' && variant === 'swap'}
	<Swap value={theme} variant="rotate">
		<slot slot="off" name="off" />
		<slot slot="on" name="on" />
	</Swap>
{/if}
