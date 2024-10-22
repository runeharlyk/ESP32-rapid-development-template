<script lang="ts">
	export let showModal: boolean;
	let dialog: HTMLDialogElement;

	$: if (dialog && showModal) dialog.showModal();
</script>

<!-- svelte-ignore a11y-click-events-have-key-events a11y-no-noninteractive-element-interactions -->
<dialog
	bind:this={dialog}
	on:close={() => (showModal = false)}
	on:click|self={() => dialog.close()}
	class="modal modal-bottom sm:modal-middle"
>
	<!-- svelte-ignore a11y-no-static-element-interactions -->
	<form on:click|stopPropagation class="modal-box" on:submit|preventDefault>
		<slot name="header" />
		<div class="py-4"><slot /></div>
		<div class="modal-action gap-2">
			<slot name="action" />
			<!-- svelte-ignore a11y-autofocus -->
			<button class="btn" autofocus on:click={() => dialog.close()}>close</button>
		</div>
	</form>
</dialog>
