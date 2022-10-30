import { AxiosPromise } from 'axios';
import { AXIOS } from '../api/endpoints';

export function restart(): AxiosPromise<void> {
  return AXIOS.post('/restart');
}

export function getSettings(): AxiosPromise<void> {
  return AXIOS.get('/getSettings');
}

export function getCustomizations(): AxiosPromise<void> {
  return AXIOS.get('/getCustomizations');
}
