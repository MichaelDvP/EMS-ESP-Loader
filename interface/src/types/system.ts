export interface SystemStatus {
  emsesp_version: string;
  esp_platform: string;
  max_alloc_heap: number;
  cpu_freq_mhz: number;
  free_heap: number;
  sdk_version: string;
  flash_chip_size: number;
  flash_chip_speed: number;
  app_used: number;
  app_free: number;
  app_max: number;
  fs_used: number;
  fs_free: number;
  uptime: string;
  free_mem: number;
  psram_size: number;
  free_psram: number;
}

export interface OTASettings {
  enabled: boolean;
  port: number;
  password: string;
}

export interface Version {
  version: string;
  url: string;
  changelog: string;
}
