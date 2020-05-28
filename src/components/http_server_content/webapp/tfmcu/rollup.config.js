// rollup.config.js
import json from '@rollup/plugin-json';
import { terser } from 'rollup-plugin-terser';
import strip from '@rollup/plugin-strip';
import { eslint } from "rollup-plugin-eslint";
import svelte from 'rollup-plugin-svelte';

const isProduction = process.env.buildTarget === "PROD";

export default {
  input: 'src/main.js',
  output: [{
    file: 'build/bundle.js',
    format: 'iife',
    name: 'tfmcu',
    plugins: [
    ]
  }, {
    file: 'build/bundle.min.js',
    format: 'iife',
    name: 'tfmcu',
    plugins: [
      terser()
    ]
  }
  ],
  plugins: [
    json(),
    eslint({
      "parserOptions": {
        "ecmaVersion": 6,
        "sourceType": "module",
        "ecmaFeatures": {
          "jsx": false
        }
      },
      "rules": {
        "semi": "error"
      }
    }),
    ...isProduction ? [
      strip({
        functions: ['testing.*', 'appDebug.*', 'console.*', 'assert.*'],
        sourceMap: true
      })] : [],
      
          svelte({
      // By default, all .svelte and .html files are compiled
      extensions: ['.my-custom-extension'],

      // You can restrict which files are compiled
      // using `include` and `exclude`
      include: 'src/components/**/*.svelte',

      // By default, the client-side compiler is used. You
      // can also use the server-side rendering compiler
      generate: 'ssr',
      
      // ensure that extra attributes are added to head
      // elements for hydration (used with ssr: true)
      hydratable: true,

      // Optionally, preprocess components with svelte.preprocess:
      // https://svelte.dev/docs#svelte_preprocess
      preprocess: {
        style: ({ content }) => {
          return transformStyles(content);
        }
      },

      // Emit CSS as "files" for other plugins to process
      emitCss: true,

      // Extract CSS into a separate file (recommended).
      // See note below
      css: function (css) {
        console.log(css.code); // the concatenated CSS
        console.log(css.map); // a sourcemap

        // creates `main.css` and `main.css.map` — pass `false`
        // as the second argument if you don't want the sourcemap
        css.write('public/main.css');
      },

      // Warnings are normally passed straight to Rollup. You can
      // optionally handle them here, for example to squelch
      // warnings with a particular code
      onwarn: (warning, handler) => {
        // e.g. don't warn on <marquee> elements, cos they're cool
        if (warning.code === 'a11y-distracting-elements') return;

        // let Rollup handle all other warnings normally
        handler(warning);
      }
    })
  ]
};
