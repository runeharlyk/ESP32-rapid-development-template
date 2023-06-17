# 🚀 ESP32-rapid-development-template

A skeleton project for rapid development using the ESP32 board

## Features

### ESP32 project

- AsyncWebserver - SPA hosting
- Websocket - Realtime communication

### 📲Web application

- Svelte
- Typescript
- Tailwind CSS
- ESLint
- Prettier
- Initial build size < 13 kb

## Getting started

1. Fork the project

1. Clone and open the new project

    ```console
    git clone https://github.com/{USERNAME}/ESP32-rapid-development-template
    ```

1. Install dependencies

    ```console
    cd app
    npm install
    ```

## Usage

### Developing

1. Run the app

    ```console
    npm run dev
    ```

## Building

1. Build the app

    ```console
    npm run build
    ```

1. Upload Filesystem Image using platformIO

## Project setup

This is the steps used to make the project template

1. Initialize [new platformIO project](https://docs.platformio.org/en/stable/integration/ide/vscode.html#quick-start) using [VS code extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

1. Use Vite to install svelte (not kit).

    ```console
    npm init vite
    ```

    ```console
    ? Project name: » app
    ```

    ```console
    ? Select a framework: » - Use arrow-keys. Return to submit.
        Vanilla
        Vue
        React
        Preact
        Lit
    >   Svelte
        Others
    ```

    ```console
    ? Select a variant: » - Use arrow-keys. Return to submit.
    >   TypeScript
        JavaScript
        SvelteKit ↗
    ```

1. Navigate to app and install dependencies

    ```console
    cd app
    npm install
    ```

1. Install and configure [vite-plugin-singlefile](https://www.npmjs.com/package/vite-plugin-singlefile) to inline all js and css in `index.html`

    ```console
    npm i vite-plugin-singlefile
    ```

    Add plugin in `vite.config.ts` and configure build destination to *../data*

    ```ts
    import { defineConfig } from 'vite'
    import { svelte } from '@sveltejs/vite-plugin-svelte'
     >  import { viteSingleFile } from 'vite-plugin-singlefile';

    // https://vitejs.dev/config/
    export default defineConfig({
     >  plugins: [svelte(), viteSingleFile()],
     >  build: {
     >      outDir: '../data',
     >      emptyOutDir: true
     >  }
    })
    ```

1. Install and configure [Tailwind CSS](https://tailwindcss.com/)

    ```console
    npm install -D tailwindcss postcss autoprefixer
    npx tailwindcss init -p
    ```

    Add content type in `tailwind.config.js`

    ```ts
    /** @type {import('tailwindcss').Config} */
    export default {
        > content: ['./src/**/*.{html,js,ts,svelte}'],
        theme: {
            extend: {},
        },
        plugins: [],
    }
    ```

    Add Tailwind directives in the top of `src/app.css`

    ```css
    @tailwind base;
    @tailwind components;
    @tailwind utilities;
    ```

1. Install [ESLint](https://eslint.org/) and [Prettier](https://prettier.io/)

    ```console
    npm init @eslint/config
    npm install --save-dev eslint-config-prettier
    npm install --save-dev --save-exact prettier
    ```

    [Install git hook](https://prettier.io/docs/en/install.html#git-hooks)

    ```console
    npm install --save-dev husky lint-staged
    npx husky install
    npm pkg set scripts.prepare="husky install"
    npx husky add .husky/pre-commit "npx lint-staged"
    ```

    Add lint-staged to `package.json`

    ```json
    {
        "lint-staged": {
            "**/*": "prettier --write --ignore-unknown"
        }
    }
    ```
