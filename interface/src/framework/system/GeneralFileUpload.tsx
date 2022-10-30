import { FC } from 'react';
import { AxiosPromise } from 'axios';

import { Typography, Box } from '@mui/material';

import { FileUploadConfig } from '../../api/endpoints';

import { SingleUpload, useFileUpload } from '../../components';

import { useI18nContext } from '../../i18n/i18n-react';

interface UploadFileProps {
  uploadGeneralFile: (file: File, config?: FileUploadConfig) => AxiosPromise<void>;
}

const GeneralFileUpload: FC<UploadFileProps> = ({ uploadGeneralFile }) => {

  const [uploadFile, cancelUpload, uploading, uploadProgress, md5] = useFileUpload({ upload: uploadGeneralFile });

  const { LL } = useI18nContext();

  return (
    <>
      {!uploading && (
        <>
          <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
            {LL.UPLOAD()}
          </Typography>
          <Box mb={2} color="warning.main">
            <Typography variant="body2">{LL.UPLOAD_TEXT()} </Typography>
          </Box>
        </>
      )}
      {md5 !== '' && (
        <Box mb={2}>
          <Typography variant="body2">{'MD5: ' + md5}</Typography>
        </Box>
      )}
      <SingleUpload onDrop={uploadFile} onCancel={cancelUpload} uploading={uploading} progress={uploadProgress} />
    </>
  );
};

export default GeneralFileUpload;
