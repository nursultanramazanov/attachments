import { delay } from 'bluebird';

export async function demo(...args) {
  await delay(100);
  console.log(...args);
}
