<script>
  import LoadingTerminal from './components/LoadingTerminal.svelte';
  let show = false;
</script>

<LoadingTerminal />

<div class="text-white text-center my-6">
  <button
    class="bg-red-900 hover:bg-red-700 text-white font-bold py-2 px-4 rounded"
    on:click={() => show = !show}
  >
    {show ? 'Masquer' : 'Qui est l’Omnissiah ?'}
  </button>

  {#if show}
    <div
      class="mt-4 p-4 bg-black bg-opacity-80 rounded shadow-lg text-left max-w-xl mx-auto text-sm"
      transition:fade
    >
      <h2 class="text-lg font-bold text-red-400 mb-2">L’Omnissiah, Dieu-Machine</h2>
      <p>
        Dans <strong>Warhammer 40K</strong>, l’Omnissiah est une divinité vénérée par le Culte Mechanicus.
        Chaque machine est sacrée, abritant un <em>Machine Spirit</em>.
      </p>
      <p class="mt-2">
        Les Tech-Priests interprètent et exécutent des rituels pour activer ou réparer ces machines,
        croyant que la technologie est une forme de foi, et que l’Omnissiah est l’avatar de cette connaissance divine.
      </p>
    </div>
  {/if}
</div>
