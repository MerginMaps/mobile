# 2018 Peter Petrik (zilolv at gmail dot com)
# GNU General Public License 2 any later version

import argparse
import os
import dropbox
from dropbox.files import WriteMode
from dropbox.exceptions import ApiError, AuthError


# inspired by
# https://github.com/dropbox/dropbox-sdk-python/blob/master/example/back-up-and-restore/backup-and-restore-example.py
class UploadError(Exception):
    pass


# Uploads contents of LOCALFILE to Dropbox
# from Dropbox's examples
# files < 150 MB, see https://stackoverflow.com/q/33810138/2838364
# files larger > 150 MB https://stackoverflow.com/a/40114617/2838364
def backup(local, remote):
    file_size = os.path.getsize(local)
    CHUNK_SIZE = 4 * 1024 * 1024

    with open(local, 'rb') as f:
        # We use WriteMode=overwrite to make sure that the settings in the file are changed on upload
        try:
            if file_size <= CHUNK_SIZE:
                dbx.files_upload(f.read(), remote, mode=WriteMode('overwrite'))
            else:
                upload_session_start_result = dbx.files_upload_session_start(f.read(CHUNK_SIZE))
                cursor = dropbox.files.UploadSessionCursor(session_id=upload_session_start_result.session_id,
                                                           offset=f.tell())
                commit = dropbox.files.CommitInfo(path=remote)

                while f.tell() < file_size:
                    if (file_size - f.tell()) <= CHUNK_SIZE:
                        dbx.files_upload_session_finish(f.read(CHUNK_SIZE),
                                                        cursor,
                                                        commit)
                    else:
                        dbx.files_upload_session_append_v2(f.read(CHUNK_SIZE),
                                                           cursor)
                        cursor.offset = f.tell()

        except ApiError as err:
            # This checks for the specific error where a user doesn't have
            # enough Dropbox space quota to upload this file
            if (err.error.is_path() and
                    err.error.get_path().reason.is_insufficient_space()):
                raise UploadError("ERROR: Cannot back up; insufficient space.")
            elif err.user_message_text:
                raise UploadError(err.user_message_text)
            else:
                raise UploadError(str(err))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Upload QGIS package to store')
    parser.add_argument('--source',
                        required=True,
                        help='file to upload')
    parser.add_argument('--destination',
                        required=True,
                        help='full filename on Dropbox where to upload file')
    parser.add_argument('--token',
                        required=True,
                        help='Dropbox APIv2 Token key in environment')
    parser.add_argument('--dry',
                        action='store_true',
                        required=False,
                        default=False,
                        help='Dry run')

    args = parser.parse_args()
    if not os.path.exists(args.source):
        raise UploadError(args.source + " does not exists")

    # parse token
    token = os.environ.get(args.token) # DO NOT print this value, it is SECRET on travis!
    if len(token) != 64:
        raise UploadError("ERROR: Looks like your Dropbox access token is not valid, should be 64 char long")

    # Create an instance of a Dropbox class, which can make requests to the API.
    dbx = dropbox.Dropbox(token)

    # Check that the access token is valid
    try:
        dbx.users_get_current_account()
    except AuthError as err:
        raise UploadError("ERROR: Invalid access token; try re-generating an access token from the app console on the web.")

    if args.dry:
        print ("dry run")
    else:
        # Create a backup of the current settings file
        backup(local=args.source, remote=args.destination)

        # Share for public
        r = dbx.sharing_create_shared_link_with_settings(args.destination)
        print("Upload " + args.source + " to " + args.destination + ":")
        # make sure last like is alone since we use the link in the
        print(r.url)
