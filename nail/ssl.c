/*
 * Nail - a mail user agent derived from Berkeley Mail.
 *
 * Copyright (c) 2000-2002 Gunnar Ritter, Freiburg i. Br., Germany.
 */
/*
 * Copyright (c) 2002
 *	Gunnar Ritter.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Gunnar Ritter
 *	and his contributors.
 * 4. Neither the name of Gunnar Ritter nor the names of his contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GUNNAR RITTER AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GUNNAR RITTER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
#ifdef	DOSCCS
static char sccsid[] = "@(#)ssl.c	1.5 (gritter) 9/2/04";
#endif
#endif /* not lint */

#include "config.h"

#ifdef	HAVE_SOCKETS
#ifdef	USE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include <openssl/x509.h>
#include <openssl/rand.h>

#include "rcv.h"
#include "extern.h"
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#ifdef	HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif	/* HAVE_ARPA_INET_H */

#include <termios.h>

/*
 * Mail -- a mail program
 *
 * SSL functions
 */

/*
 * OpenSSL client implementation according to: John Viega, Matt Messier,
 * Pravir Chandra: Network Security with OpenSSL. Sebastopol, CA 2002.
 */

enum {
	VRFY_IGNORE,
	VRFY_WARN,
	VRFY_ASK,
	VRFY_STRICT
} ssl_vrfy_level;

static void ssl_set_vrfy_level __P((const char *));
static enum okay ssl_vrfy_decide __P((void));
static int ssl_rand_init __P((void));
static SSL_METHOD *ssl_select_method __P((const char *));
static int ssl_verify_cb __P((int, X509_STORE_CTX *));
static void ssl_load_verifications __P((struct sock *));
static void ssl_certificate __P((struct sock *, const char *));
static enum okay ssl_check_host __P((const char *, struct sock *));

static void
ssl_set_vrfy_level(uhp)
	const char *uhp;
{
	char *cp;
	char *vrvar;

	ssl_vrfy_level = VRFY_ASK;
	vrvar = ac_alloc(strlen(uhp) + 12);
	strcpy(vrvar, "ssl-verify-");
	strcpy(&vrvar[11], uhp);
	if ((cp = value(vrvar)) == NULL)
		cp = value("ssl-verify");
	ac_free(vrvar);
	if (cp != NULL) {
		if (equal(cp, "strict"))
			ssl_vrfy_level = VRFY_STRICT;
		else if (equal(cp, "ask"))
			ssl_vrfy_level = VRFY_ASK;
		else if (equal(cp, "warn"))
			ssl_vrfy_level = VRFY_WARN;
		else if (equal(cp, "ignore"))
			ssl_vrfy_level = VRFY_IGNORE;
		else
			fprintf(stderr, catgets(catd, CATSET, 265,
					"invalid value of ssl-verify: %s\n"),
					cp);
	}
}

static enum okay
ssl_vrfy_decide()
{
	enum okay ok = STOP;

	switch (ssl_vrfy_level) {
	case VRFY_STRICT:
		ok = STOP;
		break;
	case VRFY_ASK:
		{
			char *line = NULL;
			size_t linesize = 0;

			fprintf(stderr, catgets(catd, CATSET, 264,
					"Continue (y/n)? "));
			if (readline(stdin, &line, &linesize) > 0 &&
					*line == 'y')
				ok = OKAY;
			else
				ok = STOP;
			if (line)
				free(line);
		}
		break;
	case VRFY_WARN:
	case VRFY_IGNORE:
		ok = OKAY;
	}
	return ok;
}

static int
ssl_rand_init()
{
	char *cp;
	int state = 0;

	if ((cp = value("ssl-rand-egd")) != NULL) {
		cp = expand(cp);
		if (RAND_egd(cp) == -1) {
			fprintf(stderr, catgets(catd, CATSET, 245,
				"entropy daemon at \"%s\" not available\n"),
					cp);
		} else
			state = 1;
	} else if ((cp = value("ssl-rand-file")) != NULL) {
		cp = expand(cp);
		if (RAND_load_file(cp, 1024) == -1) {
			fprintf(stderr, catgets(catd, CATSET, 246,
				"entropy file at \"%s\" not available\n"), cp);
		} else {
			struct stat st;

			if (stat(cp, &st) == 0 && S_ISREG(st.st_mode) &&
					access(cp, W_OK) == 0) {
				if (RAND_write_file(cp) == -1) {
					fprintf(stderr, catgets(catd, CATSET,
								247,
				"writing entropy data to \"%s\" failed\n"), cp);
				}
			}
			state = 1;
		}
	}
	return state;
}

static SSL_METHOD *
ssl_select_method(uhp)
	const char *uhp;
{
	SSL_METHOD *method;
	char *cp, *mtvar;

	mtvar = ac_alloc(strlen(uhp) + 12);
	strcpy(mtvar, "ssl-method-");
	strcpy(&mtvar[11], uhp);
	if ((cp = value(mtvar)) == NULL)
		cp = value("ssl-method");
	ac_free(mtvar);
	if (cp != NULL) {
		if (equal(cp, "ssl2"))
			method = SSLv2_client_method();
		else if (equal(cp, "ssl3"))
			method = SSLv3_client_method();
		else if (equal(cp, "tls1"))
			method = TLSv1_client_method();
		else {
			fprintf(stderr, catgets(catd, CATSET, 244,
					"Invalid SSL method \"%s\"\n"), cp);
			method = SSLv23_client_method();
		}
	} else
		method = SSLv23_client_method();
	return method;
}

static int
ssl_verify_cb(int success, X509_STORE_CTX *store)
{
	if (success == 0) {
		char data[256];
		X509 *cert = X509_STORE_CTX_get_current_cert(store);
		int depth = X509_STORE_CTX_get_error_depth(store);
		int err = X509_STORE_CTX_get_error(store);

		fprintf(stderr, catgets(catd, CATSET, 229,
				"Error with certificate at depth: %i\n"),
				depth);
		X509_NAME_oneline(X509_get_issuer_name(cert), data,
				sizeof data);
		fprintf(stderr, catgets(catd, CATSET, 230, " issuer = %s\n"),
				data);
		X509_NAME_oneline(X509_get_subject_name(cert), data,
				sizeof data);
		fprintf(stderr, catgets(catd, CATSET, 231, " subject = %s\n"),
				data);
		fprintf(stderr, catgets(catd, CATSET, 232, " err %i: %s\n"),
				err, X509_verify_cert_error_string(err));
		if (ssl_vrfy_decide() != OKAY)
			return 0;
	}
	return 1;
}

static void
ssl_load_verifications(sp)
	struct sock *sp;
{
	char *ca_dir, *ca_file;

	if (ssl_vrfy_level == VRFY_IGNORE)
		return;
	if ((ca_dir = value("ssl-ca-dir")) != NULL)
		ca_dir = expand(ca_dir);
	if ((ca_file = value("ssl-ca-file")) != NULL)
		ca_file = expand(ca_file);
	if (ca_dir || ca_file) {
		if (SSL_CTX_load_verify_locations(sp->s_ctx,
					ca_file, ca_dir) != 1) {
			fprintf(stderr, catgets(catd, CATSET, 233,
						"Error loading"));
			if (ca_dir) {
				fprintf(stderr, catgets(catd, CATSET, 234,
							" %s"), ca_dir);
				if (ca_file)
					fprintf(stderr, catgets(catd, CATSET,
							235, " or"));
			}
			if (ca_file)
				fprintf(stderr, catgets(catd, CATSET, 236,
						" %s"), ca_file);
			fprintf(stderr, catgets(catd, CATSET, 237, "\n"));
		}
	}
	if (value("ssl-no-default-ca") == NULL) {
		if (SSL_CTX_set_default_verify_paths(sp->s_ctx) != 1)
			fprintf(stderr, catgets(catd, CATSET, 243,
				"Error loading default CA locations\n"));
	}
	SSL_CTX_set_verify(sp->s_ctx, SSL_VERIFY_PEER, ssl_verify_cb);
}

static void
ssl_certificate(sp, uhp)
	struct sock *sp;
	const char *uhp;
{
	char *certvar, *keyvar, *cert, *key;

	certvar = ac_alloc(strlen(uhp) + 10);
	strcpy(certvar, "ssl-cert-");
	strcpy(&certvar[9], uhp);
	if ((cert = value(certvar)) != NULL ||
			(cert = value("ssl-cert")) != NULL) {
		cert = expand(cert);
		if (SSL_CTX_use_certificate_chain_file(sp->s_ctx, cert) == 1) {
			keyvar = ac_alloc(strlen(uhp) + 9);
			strcpy(keyvar, "ssl-key-");
			if ((key = value(keyvar)) == NULL &&
					(key = value("ssl-key")) == NULL)
				key = cert;
			else
				key = expand(key);
			if (SSL_CTX_use_PrivateKey_file(sp->s_ctx, key,
						SSL_FILETYPE_PEM) != 1)
				fprintf(stderr, catgets(catd, CATSET, 238,
				"cannot load private key from file %s\n"),
						key);
			ac_free(keyvar);
		} else
			fprintf(stderr, catgets(catd, CATSET, 239,
				"cannot load certificate from file %s\n"),
					cert);
	}
	ac_free(certvar);
}

static enum okay
ssl_check_host(server, sp)
	const char *server;
	struct sock *sp;
{
	X509 *cert;
	X509_NAME *subj;
	char data[256];
	/*GENERAL_NAMES*/STACK	*gens;
	GENERAL_NAME	*gen;
	int	i;

	if ((cert = SSL_get_peer_certificate(sp->s_ssl)) == NULL) {
		fprintf(stderr, catgets(catd, CATSET, 248,
				"no certificate from \"%s\"\n"), server);
		return STOP;
	}
	gens = X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
	if (gens != NULL) {
		for (i = 0; i < sk_GENERAL_NAME_num(gens); i++) {
			gen = sk_GENERAL_NAME_value(gens, i);
			if (gen->type == GEN_DNS &&
					!asccasecmp(gen->d.ia5->data,
						(char *)server))
				goto found;
		}
	}
	if ((subj = X509_get_subject_name(cert)) != NULL &&
			X509_NAME_get_text_by_NID(subj, NID_commonName,
				data, sizeof data) > 0) {
		data[sizeof data - 1] = 0;
		if (asccasecmp(data, server) == 0)
			goto found;
	}
	X509_free(cert);
	return STOP;
found:	X509_free(cert);
	return OKAY;
}

enum okay
ssl_open(server, sp, uhp)
	const char *server;
	struct sock *sp;
	const char *uhp;
{
	static int initialized, rand_init;
	char *cp;
	long options;

	if (initialized == 0) {
		SSL_library_init();
		initialized = 1;
	}
	if (rand_init == 0)
		rand_init = ssl_rand_init();
	ssl_set_vrfy_level(uhp);
	if ((sp->s_ctx = SSL_CTX_new(ssl_select_method(uhp))) == NULL) {
		ssl_gen_err(catgets(catd, CATSET, 261, "SSL_CTX_new() failed"));
		return STOP;
	}
#ifdef	SSL_MODE_AUTO_RETRY
	/* available with OpenSSL 0.9.6 or later */
	SSL_CTX_set_mode(sp->s_ctx, SSL_MODE_AUTO_RETRY);
#endif	/* SSL_MODE_AUTO_RETRY */
	options = SSL_OP_ALL;
	if (value("ssl-v2-allow") == NULL)
		options |= SSL_OP_NO_SSLv2;
	SSL_CTX_set_options(sp->s_ctx, options);
	ssl_load_verifications(sp);
	ssl_certificate(sp, uhp);
	if ((cp = value("ssl-cipher-list")) != NULL) {
		if (SSL_CTX_set_cipher_list(sp->s_ctx, cp) != 1)
			fprintf(stderr, catgets(catd, CATSET, 240,
					"invalid ciphers: %s\n"), cp);
	}
	if ((sp->s_ssl = SSL_new(sp->s_ctx)) == NULL) {
		ssl_gen_err(catgets(catd, CATSET, 262, "SSL_new() failed"));
		return STOP;
	}
	SSL_set_fd(sp->s_ssl, sp->s_fd);
	if (SSL_connect(sp->s_ssl) < 0) {
		ssl_gen_err(catgets(catd, CATSET, 263,
				"could not initiate SSL/TLS connection"));
		return STOP;
	}
	if (ssl_vrfy_level != VRFY_IGNORE) {
		if (ssl_check_host(server, sp) != OKAY) {
			fprintf(stderr, catgets(catd, CATSET, 249,
				"host certificate does not match \"%s\"\n"),
				server);
			if (ssl_vrfy_decide() != OKAY)
				return STOP;
		}
	}
	return OKAY;
}

void
ssl_gen_err(msg)
	const char *msg;
{
	SSL_load_error_strings();
	fprintf(stderr, "%s: %s\n", msg,
		ERR_error_string(ERR_get_error(), NULL));
}

#endif	/* USE_SSL */
#endif	/* HAVE_SOCKETS */
