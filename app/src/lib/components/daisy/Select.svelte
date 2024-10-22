<script lang="ts">
	export let options: string[];
	export let placeholder: string | undefined;
</script>

<label class="form-control w-full max-w-xs">
	{#if $$slots.header}
		<div class="label">
			<span class="label-text">
				<slot name="header" />
			</span>
		</div>
	{/if}
	<select class="select select-bordered {$$restProps.class}" on:select>
		{#if placeholder}
			<option disabled selected>{placeholder}</option>
		{/if}
		{#each options as option}
			<option>{option}</option>
		{/each}
	</select>
	{#if $$slots.footer}
		<div class="label">
			<span class="label-text">
				<slot name="footer" />
			</span>
		</div>
	{/if}
</label>
