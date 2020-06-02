'use strict';
import { writable, derived } from 'svelte/store';
import { OptionStore} from './custom_stores.js';


export const Pcts = OptionStore();
export const Gmu = writable( [0, 7, 7, 7, 7, 7, 7, 7] );
export const McuConfig = OptionStore();

export const McuConfigKeys = derived(McuConfig, mcuConfig => Object.keys(mcuConfig) || []); // XXX: ordering of keys?
