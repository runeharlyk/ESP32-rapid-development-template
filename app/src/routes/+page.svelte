<script lang="ts">
	import type { PageData } from './$types';
	import logo from '$lib/assets/logo.png';

	export let data: PageData;
</script>

<div class="hero bg-base-100 h-screen">
	<div class="card md:card-side bg-base-200 shadow-2xl">
		<figure class="bg-base-200"><img src={logo} alt="Logo" class="h-auto w-64" /></figure>
		<div class="card-body w-80">
			<h2 class="card-title text-center text-2xl">Welcome to you Hamster Pedometer</h2>
			<p class="py-6 text-center">
				Start by navigation to
				<a class="link" href="/pedometer">Pedometer</a>
			</p>
		</div>
	</div>
</div>
