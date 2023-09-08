import { FC } from 'react';

import { CssBaseline } from '@mui/material';
import { createTheme, responsiveFontSizes, ThemeProvider } from '@mui/material/styles';
// import { blueGrey, blue } from '@mui/material/colors';

import { RequiredChildrenProps } from './utils';

const theme = responsiveFontSizes(
  createTheme({
    typography: {
      fontSize: 13
    },
    palette: {
      // mode: 'dark',
      secondary: {
        main: '#2196f3' // blue[500]
      },
      info: {
        main: '#607d8b' // blueGrey[500]
      }
    }
  })
);

const CustomTheme: FC<RequiredChildrenProps> = ({ children }) => (
  <ThemeProvider theme={theme}>
    <CssBaseline />
    {children}
  </ThemeProvider>
);

export default CustomTheme;
