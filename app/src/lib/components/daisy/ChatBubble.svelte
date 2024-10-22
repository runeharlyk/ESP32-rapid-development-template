<script lang="ts">
	import { Colors, Direction } from './types';

	export let variant: Direction;
	export let color: 'none' | Colors;
</script>

<div
	class="chat {$$restProps.class}"
	class:chat-start={variant === 'left'}
	class:chat-end={variant === 'right'}
>
	{#if $$slots.icon}
		<div class="chat-image avatar">
			<div class="w-10 rounded-full">
				<slot name="icon" />
			</div>
		</div>
	{/if}

	{#if $$slots.header}
		<div class="chat-header">
			<slot name="header" />
		</div>
	{/if}

	<div
		class="chat-bubble"
		class:chat-bubble-secondary={color === 'secondary'}
		class:chat-bubble-primary={color === 'primary'}
		class:chat-bubble-accent={color === 'accent'}
		class:chat-bubble-info={color === 'info'}
		class:chat-bubble-success={color === 'success'}
		class:chat-bubble-warning={color === 'warning'}
		class:chat-bubble-error={color === 'error'}
	>
		<slot />
	</div>

	{#if $$slots.footer}
		<div class="chat-footer">
			<slot name="footer" />
		</div>
	{/if}
</div>
