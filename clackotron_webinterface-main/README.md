# Clackotron 2000 - Webinterface
This repository contains the webinterface for the Clackotron 2000 project, which provides a simple, all-in-one solution for controlling SBB split-flap modules. For more information on the project in general, visit the [docs repository](https://github.com/clackotron/clackotron_docs).

The webinterface is based on Svelte.js and built using the Vite tooling. There is currently no automated testing in place.

## Development
For development, simply run `npm i` to install all dependencies and `npm run dev` to run a development server. The webinterface will attempt to load data from `/config` once every second upon loading.

## Deployment
For use on the actual device hardware, build the code using `npm run build` and copy the contents of the `dist` directory to the `data` directory in your Clackotron 2000 firmware. 

If you don't want to go through the trouble of building the webinterface on your own, use one of the releases in this repository.
