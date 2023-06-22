<script lang="ts">
  import Counter from './components/Counter.svelte'

  import { connect } from "./lib/socket";
  import location from "./lib/location";

  connect("ws://" + location + "/ws")
</script>

<svelte:head>
    <title>ESP32 template project</title>
</svelte:head>

<main>
  <h1>Vite + Svelte + TS + Tailwind</h1>

  <div class="card">
    <Counter />
  </div>

  <p>
    Check out <a href="https://github.com/sveltejs/kit#readme" target="_blank" rel="noreferrer">SvelteKit</a>, the official Svelte app framework powered by Vite!
  </p>

  <p class="read-the-docs">
    Click on the Vite and Svelte logos to learn more
  </p>
</main>

<style>
  .read-the-docs {
    color: #888;
  }
</style>
