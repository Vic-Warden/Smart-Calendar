<script>
    import TacticalViewer from '$lib/TacticalViewer.svelte';
    let showViewer = false;
  </script>
  
  <div class="fixed bottom-6 right-6 z-50">
    <button
      on:click={() => showViewer = !showViewer}
      class="bg-blue-900 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded shadow-lg border border-blue-500"
    >
      {showViewer ? 'Fermer' : '[ASTRA MILITARUM] - Inspection du Modèle Tactique'}
    </button>
  </div>
  
  {#if showViewer}
    <div class="fixed inset-0 bg-black bg-opacity-90 z-40 flex items-center justify-center p-6">
      <div class="bg-zinc-900 p-6 rounded-lg shadow-xl border border-blue-500 flex flex-col md:flex-row gap-8 max-w-6xl">
        
        <div class="flex-1 text-left text-blue-400 font-mono text-lg uppercase">
          <h2>Modèle Tactique</h2>
          <p class="mt-4 text-sm text-blue-200">
            Inspection visuelle en cours... Rapport de stabilité et d’armement en cours de synchronisation avec le cogitateur impérial.
          </p>
        </div>
  
        <div class="flex-1">
          <TacticalViewer />
        </div>
      </div>
    </div>
  {/if}
  