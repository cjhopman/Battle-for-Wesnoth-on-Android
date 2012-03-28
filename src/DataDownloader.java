package com.androthsoft.battle;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;
import android.text.SpannedString;
import android.util.Log;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.os.Environment;
import android.widget.TextView;

import java.security.cert.*;
import java.security.SecureRandom;
import java.util.zip.*;
import java.io.*;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.String;

import org.jets3t.service.impl.rest.httpclient.RestS3Service;
import org.jets3t.service.model.S3Object;
import org.jets3t.service.ServiceException;

class DataDownloader extends Thread {
	class StatusWriter
	{
		private TextView status_;
		private MainActivity parent_;
		private SpannedString oldText = new SpannedString("");

		public StatusWriter( TextView status, MainActivity parent )
		{
			status_ = status;
			parent_ = parent;
		}
		public void setparent_( TextView status, MainActivity parent )
		{
			synchronized(DataDownloader.this) {
				status_ = status;
				parent_ = parent;
				setText( oldText.toString() );
			}
		}

		public void setText(final String str)
		{
			class Callback implements Runnable
			{
				TextView status_;
				SpannedString text_;
				Callback(TextView st, SpannedString t) {
					status_ = st;
					text_ = t;
				}
				public void run()
				{
					status_.setText(text_);
				}
			}
			synchronized(DataDownloader.this) {
				Callback cb = new Callback(status_, new SpannedString(str));
				oldText = new SpannedString(str);
				if( parent_ != null && status_ != null )
					parent_.runOnUiThread(cb);
			}
		}

	}

	public DataDownloader( MainActivity parent, TextView status )
	{
		parent_ = parent;
		status_ = new StatusWriter( status, parent );
		outFilesDir = Globals.DataDir;
		DownloadComplete = false;
		this.start();
	}

	public void setStatusField(TextView status)
	{
		synchronized(this) {
			status_.setparent_( status, parent_ );
		}
	}

	@Override
		public void run()
		{
			Globals.setDownloadUrls();
			for (int i = 0; i < Globals.DataDownloadUrl.length; i++) {
				String[] DownloadDesc = Globals.DataDownloadUrl[i].split("[|]");
				if (!DownloadDataFile(DownloadDesc)) {
					DownloadFailed = true;
					return;
				}
			}

			DownloadComplete = true;
			initparent_();
		}

	public OutputStream getFileOutputStream(String path) {
		try {
			new File(path.substring(0, path.lastIndexOf("/"))).mkdirs();
			return new FileOutputStream(path);
		} catch(FileNotFoundException e) {
			System.out.println("Saving file '" + path + "' - cannot create file: " + e.toString());
		} catch(SecurityException e) {
			System.out.println("Saving file '" + path + "' - cannot create file: " + e.toString());
		}
		return null;
	}

	public InputStream getS3Stream(String bucket, String file) {
		try {
			S3Object object = new RestS3Service(null).getObject(bucket, file);
			return object.getDataInputStream();
		} catch (ServiceException e) {
			e.printStackTrace();
			return null;
		}
	}

	public boolean saveStream(InputStream stream, String url) {
		OutputStream file = null;
		try {
			file = getFileOutputStream(url);
			int len;
			byte[] buf = new byte[4096];
			while (true) {
				len = stream.read(buf);
				if (len < 0) break;
				file.write(buf, 0, len);
			}
			file.flush();
			file.close();
			stream.close();
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	public boolean untarStream(InputStream stream, String url, int totalLen, byte[] buf) {
		Resources res = parent_.getResources();
		System.out.println("Reading from tar file '" + url + "'");
		TarInputStream tar = new TarInputStream(stream);

		while(true) {
			TarEntry entry = null;

			try {
				entry = tar.getNextEntry();
			} catch(java.io.IOException e) {
				status_.setText( res.getString(R.string.error_dl_from, url) );
				return false;
			}

			if( entry == null ) {
				break;
			}

			String path = getOutFilePath(entry.getName());
			status_.setText("Extracting: " + entry.getName());

			if (entry.isDirectory()) {
				new File(path).mkdirs();
				System.out.println("Handled: " + path);
				continue;
			}

			OutputStream out = getFileOutputStream(path);

			try {
				tar.copyEntryContents(out);
				out.flush();
				out.close();
			} catch( java.io.IOException e ) {
				status_.setText( res.getString(R.string.error_write, path) );
				System.out.println("Saving file '" + path + "' - error writing or downloading: " + e.toString());
				e.printStackTrace();
				return false;
			}

			System.out.println("Saving file '" + path + "' done");
		}
		System.out.println("Reading from tar file '" + url + "' finished");
		return true;
	}
	public boolean unzipStream(InputStream stream, String url, int totalLen, byte[] buf) {
		Resources res = parent_.getResources();
		System.out.println("Reading from zip file '" + url + "'");
		ZipInputStream zip = new ZipInputStream(stream);

		while(true) {
			ZipEntry entry = null;

			try {
				entry = zip.getNextEntry();
			} catch(java.io.IOException e) {
				status_.setText( res.getString(R.string.error_dl_from, url) );
				return false;
			}

			if( entry == null ) {
				break;
			}

			String path = getOutFilePath(entry.getName());

			if (entry.isDirectory()) {
				new File(path).mkdirs();
				continue;
			}
			
			try {
				CheckedInputStream check = new CheckedInputStream( new FileInputStream(path), new CRC32() );
				while( check.read(buf, 0, buf.length) > 0 ) {};
				check.close();
				if(check.getChecksum().getValue() == entry.getCrc()) {
					continue;
				}
				File ff = new File(path);
				ff.delete();
			} catch(Exception e) {
			}

			OutputStream out = getFileOutputStream(path);

			try {
				int len = zip.read(buf);
				while (len >= 0)
				{
					if(len > 0)
						out.write(buf, 0, len);
					len = zip.read(buf);

					status_.setText("Extracting: " + entry.getName());
				}
				out.flush();
				out.close();
				out = null;
			} catch( java.io.IOException e ) {
				status_.setText( res.getString(R.string.error_write, path) );
				System.out.println("Saving file '" + path + "' - error writing or downloading: " + e.toString());
				return false;
			}

			System.out.println("Saving file '" + path + "' done");
		}
		System.out.println("Reading from zip file '" + url + "' finished");
		return true;
	}

	public String getFlagFilename(String assetName) {
		return getOutFilePath(assetName) + "-finished";
	}

	public String getAssetVersion(String assetName) {
		BufferedReader r = null;
		try {
			File f = new File(getFlagFilename(assetName));
			if (f.exists() && !f.isFile()) {
				return "0";
			}
			r = new BufferedReader(new FileReader(f));
			String ret = r.readLine();
			Integer.parseInt(ret);
			return ret;
		} catch (SecurityException e) {
			return "-1";
		} catch (FileNotFoundException e) {
			return "-2";
		} catch (IOException e) {
			return "0";
		} catch (NumberFormatException e) {
			return "0";
		} finally {
			try {
				if (r != null) r.close();
			} catch (IOException e) {
			}
		}
	}

	public void setAssetFinished(String assetName) {
		PrintWriter w = null;
		try {
			File f = new File(getFlagFilename(assetName));
			f.mkdirs();
			if (f.exists()) f.delete();
			w = new PrintWriter(new FileWriter(f));
			w.println(Globals.ApplicationVersion);
		} catch(SecurityException e) {
		} catch(FileNotFoundException e) {
		} catch(IOException e) {
		} finally {
			if (w != null) w.close();
		}
	}

	public boolean DownloadDataFile(String[] dataDownloadDesc)
	{
		if(dataDownloadDesc.length < 3)
			return false;
		boolean isS3 = dataDownloadDesc[1].equals("s3");

		String dataDownloadUrl = "desc|" + dataDownloadDesc[isS3 ? 3 : 2];
		String [] downloadUrls = dataDownloadUrl.split("[|]");
		if(downloadUrls.length < 2)
			return false;

		String url = downloadUrls[1];
		Resources res = parent_.getResources();
		
		int oldVersion = Integer.parseInt(getAssetVersion(url));
		int versionIdx = isS3 ? 4 : 3;
		int requiredVersion = dataDownloadDesc.length < versionIdx + 1 ? 0 : Integer.parseInt(dataDownloadDesc[versionIdx]);
		if (oldVersion >= requiredVersion) {
			status_.setText( res.getString(R.string.download_unneeded) );
			return true;
		}

		// Create output directory (not necessary for phone storage)
		System.out.println("Downloading data to: '" + outFilesDir + "'");
		try {
			File f = new File(getOutFilePath(".nomedia"));
			f.mkdirs();
			f.createNewFile();
		}
		catch( SecurityException e ) {}
		catch( FileNotFoundException e ) {}
		catch( IOException e ) {};

		int totalLen = 0;
		InputStream stream = null;
		byte[] buf = new byte[16384];

		status_.setText( res.getString(R.string.connecting_to, url) );

		System.out.println("Fetching file from assets: " + url);

		System.out.println("Unpacking from assets: '" + url + "'");

		if (dataDownloadDesc[1].equals("res")) {
			try {
				stream = parent_.getAssets().open(url);
			} catch( IOException e ) {
				status_.setText( res.getString(R.string.error_dl_from, url) );
				return false;
			}
		} else if (dataDownloadDesc[1].equals("s3")) {
			stream = getS3Stream(dataDownloadDesc[2], dataDownloadDesc[3]);
		}

		if (dataDownloadDesc[0].equals("zip")) {
			if (!unzipStream(stream, url, totalLen, buf)) {
				return false;
			}
		} else if (dataDownloadDesc[0].equals("tar")) {
			if (!untarStream(stream, url, totalLen, buf)) {
				return false;
			}
		} else if (dataDownloadDesc[0].equals("raw")) {
			if (!saveStream(stream, getOutFilePath(url))) {
				return false;
			}
		} else {
			return false;
		}

		setAssetFinished(url);
		status_.setText( res.getString(R.string.dl_finished) );

		try {
			stream.close();
		} catch( java.io.IOException e ) {
		}

		return true;
	};

	private void initparent_()
	{
		class Callback implements Runnable
		{
			public MainActivity parent_;
			public void run()
			{
				parent_.initSDL();
			}
		}
		Callback cb = new Callback();
		synchronized(this) {
			cb.parent_ = parent_;
			if(parent_ != null)
				parent_.runOnUiThread(cb);
		}
	}

	private String getOutFilePath(final String filename)
	{
		return outFilesDir + "/" + filename;
	};

	public StatusWriter status_;
	public boolean DownloadComplete = false;
	public boolean DownloadFailed = false;
	private MainActivity parent_;
	private String outFilesDir = null;
}
