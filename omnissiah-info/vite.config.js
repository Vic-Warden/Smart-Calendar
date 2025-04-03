import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte'

export default defineConfig(
  {
  plugins: [svelte()],
  build: 
  {
    lib: 
    {
      entry: 'src/main.js',
      name: 'OmnissiahInfo',
      fileName: 'index',
      formats: ['es']
    },
    rollupOptions: 
    {
      output: 
      {
        inlineDynamicImports: true
      }
    }
  }
})
