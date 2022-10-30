import { FC } from 'react';
import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, Theme, useTheme } from '@mui/material';

import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import RefreshIcon from '@mui/icons-material/Refresh';

import { ButtonRow, FormLoader, SectionContent } from '../../components';
import { MqttStatus } from '../../types';
import * as MqttApi from '../../api/mqtt';
import { useRest } from '../../utils';

import { useI18nContext } from '../../i18n/i18n-react';

export const mqttStatusHighlight = ({ enabled }: MqttStatus, theme: Theme) => {
  if (!enabled) {
    return theme.palette.info.main;
  }
  return theme.palette.success.main;
};


const MqttStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<MqttStatus>({ read: MqttApi.readMqttStatus });

  const { LL } = useI18nContext();

  const theme = useTheme();

  const mqttStatus = ({ enabled }: MqttStatus) => {
    if (!enabled) {
      return LL.NOT_ENABLED();
    }
    return LL.ACTIVE();
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const renderConnectionStatus = () => {
      return (
        <>
          <ListItem>
            <ListItemAvatar>
              <Avatar>#</Avatar>
            </ListItemAvatar>
            <ListItemText primary="Client ID" secondary={data.client_id} />
          </ListItem>
          <Divider variant="inset" component="li" />
        </>
      );
    };

    return (
      <>
        <List>
          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: mqttStatusHighlight(data, theme) }}>
                <DeviceHubIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.STATUS_OF("")} secondary={mqttStatus(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          {data.enabled && renderConnectionStatus()}
        </List>
        <ButtonRow>
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
            {LL.REFRESH()}
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title={LL.STATUS_OF("MQTT")} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default MqttStatusForm;
